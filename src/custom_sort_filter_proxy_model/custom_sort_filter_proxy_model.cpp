#include "custom_sort_filter_proxy_model.h"

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
{}

bool CustomSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	return _hasMatchingDescendant(source_row, source_parent);
}

bool CustomSortFilterProxyModel::_hasMatchingDescendant(int source_row, const QModelIndex& source_parent) const
{
	// Checking source element accurate the filter.
	if (QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent))
	{
		return true;
	}

	QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);

	// If element has't children, there is nothing to check further.
	if (!sourceModel()->hasChildren(sourceIndex))
	{
		return false;
	}

	int childCount = sourceModel()->rowCount(sourceIndex);

	// Checking all direct children.
	for (int i = 0; i < childCount; ++i)
	{
		if (_hasMatchingDescendant(i, sourceIndex))
		{
			return true;
		}
	}

	return false;
}
