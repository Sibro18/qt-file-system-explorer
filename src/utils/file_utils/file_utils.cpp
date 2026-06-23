#include <QVector>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>

#include "file_utils.h"

qint64 FileUtils::calculateDirSize(const QString& path, QDir::Filters filters)
{
	qint64 totalSize = 0;
	QDirIterator it(
		path,
		filters,
		QDirIterator::Subdirectories
	);

	while (it.hasNext())
	{
		const QFileInfo info = it.next();

		if (!info.isDir())
		{
			totalSize += info.size();
		}
	}

	return totalSize;
}
