#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QString>
#include <QtGlobal>
#include <QDir>

class FileUtils
{
public:
	static qint64 calculateDirSize(const QString& path, QDir::Filters filters);
};

#endif // FILE_UTILS_H
