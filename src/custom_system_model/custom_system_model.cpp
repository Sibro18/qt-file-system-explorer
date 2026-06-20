#include <QFileInfo>
#include <QDir>
#include <QLocale>

#include "custom_system_model.h"

CustomSystemModel::CustomSystemModel(QThreadPool* threadPool, QObject* parent)
	: QFileSystemModel(parent),
	  _threadPool{threadPool ? threadPool : QThreadPool::globalInstance()}
{}

QVariant CustomSystemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QFileSystemModel::data(index, role);
	}

	if (index.column() == 1)
	{
		if (role == Qt::TextAlignmentRole)
		{
			return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
		}

		const QString path = filePath(index);

		if (role == CustomRoles::InProgressRole)
		{
			QReadLocker lock(&_cacheMutex);
			return _inProgress.contains(path);
		}
		else if (role == Qt::DisplayRole)
		{
			QString sizeValue;
			{
				QReadLocker lock(&_cacheMutex);
				sizeValue = _sizeCache.value(path);
			}

			return !sizeValue.isEmpty() ? sizeValue : QFileSystemModel::data(index, role);
		}
	}

	return QFileSystemModel::data(index, role);
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

	QString path = info.absoluteFilePath();

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

		qint64 size = _calculateDirSize(path);
		{
			QWriteLocker lock(&_cacheMutex);
			_inProgress.remove(path);
			_sizeCache[path] = _formatSize(size);
		}

		QMetaObject::invokeMethod(this, [this, persistentIndex]() {
			if (persistentIndex.isValid())
			{
				emit dataChanged(persistentIndex, persistentIndex, {Qt::DisplayRole, CustomRoles::InProgressRole});
			}
		}, Qt::QueuedConnection);
	});
}

qint64 CustomSystemModel::_calculateDirSize(const QString& path) const
{
	qint64 totalSize = 0;
	QVector<QString> dirsToVisit;
	dirsToVisit.reserve(256);

	dirsToVisit.push_back(path);

	while (!dirsToVisit.isEmpty())
	{
		const QString currentDir = dirsToVisit.takeLast();

		QDirIterator it(currentDir,
			QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
			QDirIterator::NoIteratorFlags
		);

		while (it.hasNext())
		{
			it.next();
			const QFileInfo info = it.fileInfo();

			// ignore symlinks.
			if (info.isSymLink())
			{
				continue;
			}

			if (info.isDir())
			{
				dirsToVisit.push_back(info.filePath());
			}
			else
			{
				totalSize += info.size();
			}
		}
	}

	return totalSize;
}

QString CustomSystemModel::_formatSize(qint64 bytes) const
{
	int precision = 2;
	if (bytes < 1024)
	{
		precision = 0;
	}
	else if (bytes < 1024LL * 1024 * 1024)
	{
		precision = 1;
	}

	return QLocale::system().formattedDataSize(bytes, precision, QLocale::DataSizeIecFormat);
}
