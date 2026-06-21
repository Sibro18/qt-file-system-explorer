#include "custom_sort_filter_proxy_model.h"

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
{}

bool CustomSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);

	// Guard against invalid idx's
	if (!sourceIndex.isValid())
	{
		return false;
	}

	// if item not present in proxy model, search only in children.
	if (!source_parent.isValid())
	{
		return _hasMatchingDescendantInChildren(sourceIndex);
	}

	// For other items: check self or descendants
	return _hasMatchingDescendant(source_row, source_parent);
}

bool CustomSortFilterProxyModel::_hasMatchingDescendant(int source_row, const QModelIndex& source_parent) const
{
	QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);

	if (!sourceIndex.isValid())
	{
		return false;
	}

	// If the item itself matches the filter, accept it immediately
	if (QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent))
	{
		return true;
	}

	// Otherwise, check if any descendant matches.
	return _hasMatchingDescendantInChildren(sourceIndex);
}

bool CustomSortFilterProxyModel::_hasMatchingDescendantInChildren(const QModelIndex& parentIndex) const
{
	// if source invalid or not has children => not accurate.
	if (!parentIndex.isValid() || !sourceModel()->hasChildren(parentIndex))
	{
		return false;
	}

	// Use a stack for iterative depth-first search.
	QVector<QModelIndex> nodesToCheck;

	// Initialize stack with direct children.
	for (int i = 0; i < sourceModel()->rowCount(parentIndex); i++)
	{
		QModelIndex child = sourceModel()->index(i, 0, parentIndex);

		if (child.isValid())
		{
			nodesToCheck.push_back(child);
		}
	}

	while (!nodesToCheck.isEmpty())
	{
		QModelIndex currentIdx = nodesToCheck.takeLast();

		// Check if current node matches the filter.
		if (QSortFilterProxyModel::filterAcceptsRow(currentIdx.row(), currentIdx.parent()))
		{
			return true;
		}

		// Push children to stack for further checking.
		if (sourceModel()->hasChildren(currentIdx))
		{
			for (int i = 0; i < sourceModel()->rowCount(currentIdx); i++)
			{
				QModelIndex subChild = sourceModel()->index(i, 0, currentIdx);

				if (subChild.isValid())
				{
					nodesToCheck.push_back(subChild);
				}
			}
		}
	}

	return false;
}
