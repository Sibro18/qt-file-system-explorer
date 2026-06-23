#include <QFileInfo>

#include "custom_system_model.h"
#include "utils/format_utils/format_utils.h"
#include "utils/file_utils/file_utils.h"

CustomSystemModel::CustomSystemModel(QThreadPool* threadPool, QObject* parent)
	: QFileSystemModel(parent),
	  _threadPool{threadPool ? threadPool : QThreadPool::globalInstance()}
{}

QVariant CustomSystemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.column() != 1)
	{
		return QFileSystemModel::data(index, role);
	}

	switch (role)
	{
		case Qt::TextAlignmentRole:
			return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);

		case CustomRoles::SizeInBytesRole:
			return _getRawSize(index);

		case CustomRoles::InProgressRole:
			return _getInProgressStatus(index);

		case Qt::DisplayRole:
			return _getFormattedSize(index, role);

		default:
			return QFileSystemModel::data(index, role);
	}
}

QVariant CustomSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::TextAlignmentRole)
	{
		return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
	}

	return QFileSystemModel::headerData(section, orientation, role);
}

void CustomSystemModel::calculateAndSetDirSize(const QModelIndex& index)
{
	if (!index.isValid())
	{
		return;
	}

	QFileInfo info = fileInfo(index);

	if (!info.isDir())
	{
		return;
	}

	QString path = filePath(index);

	{
		QWriteLocker lock(&_cacheMutex);

		if (_inProgress.contains(path))
		{
			return;
		}

		_inProgress.insert(path);
	}

	emit dataChanged(index, index, {CustomRoles::InProgressRole});

	_threadPool->start([this, path, persistentIndex = QPersistentModelIndex(index)]() {
		if (!persistentIndex.isValid())
		{
			QWriteLocker lock(&_cacheMutex);
			_inProgress.remove(path);

			return;
		}

		qint64 size = FileUtils::calculateDirSize(path, filter());
		{
			QWriteLocker lock(&_cacheMutex);
			_inProgress.remove(path);
			_sizeCache[path] = size;
		}

		emit dataChanged(persistentIndex, persistentIndex, {Qt::DisplayRole, CustomRoles::InProgressRole});
	});
}

QVariant CustomSystemModel::_getRawSize(const QModelIndex& index) const
{
	const auto fInfo = fileInfo(index);

	if (fInfo.isDir())
	{
		QReadLocker lock(&_cacheMutex);
		return _sizeCache.value(filePath(index));
	}
	else
	{
		return fInfo.size();
	}
}

QVariant CustomSystemModel::_getInProgressStatus(const QModelIndex& index) const
{
	QReadLocker lock(&_cacheMutex);
	return _inProgress.contains(filePath(index));
}

QVariant CustomSystemModel::_getFormattedSize(const QModelIndex& index, int role) const
{
	constexpr qint64 kDefaultSizeValue = -1;

	qint64 sizeValue = kDefaultSizeValue;
	{
		QReadLocker lock(&_cacheMutex);
		sizeValue = _sizeCache.value(filePath(index), kDefaultSizeValue);
	}

	return sizeValue != kDefaultSizeValue
		? FormatUtils::formatSize(sizeValue)
		: QFileSystemModel::data(index, role);
}
