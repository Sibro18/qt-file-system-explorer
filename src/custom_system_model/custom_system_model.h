#ifndef CUSTOM_SYSTEM_MODEL_H
#define CUSTOM_SYSTEM_MODEL_H

#include <QFileSystemModel>
#include <QVariant>
#include <QModelIndex>
#include <QThreadPool>
#include <QReadWriteLock>
#include <QHash>
#include <QSet>

class CustomSystemModel : public QFileSystemModel
{
	Q_OBJECT
public:
	enum CustomRoles
	{
		InProgressRole = Qt::UserRole + 1,
	};

	explicit CustomSystemModel(QThreadPool* threadPool, QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	void calculateAndSetDirSize(const QModelIndex& index);
private:
	mutable QReadWriteLock _cacheMutex;
	QHash<QString, QString> _sizeCache;
	QSet<QString> _inProgress;
	QThreadPool* _threadPool;

	qint64 _calculateDirSize(const QString& path) const;
	QString _formatSize(qint64 bytes) const;
};

#endif // CUSTOM_SYSTEM_MODEL_H
