#ifndef CUSTOM_SORT_FILTER_PROXY_MODEL_H
#define CUSTOM_SORT_FILTER_PROXY_MODEL_H

#include <QSortFilterProxyModel>
#include <QModelIndex>

class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
public:
	explicit CustomSortFilterProxyModel(QObject *parent = nullptr);
protected:
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
private:
	bool _hasMatchingDescendant(int source_row, const QModelIndex& source_parent) const;
	bool _hasMatchingDescendantInChildren(const QModelIndex& parentIndex) const;
};

#endif // CUSTOM_SORT_FILTER_PROXY_MODEL_H
