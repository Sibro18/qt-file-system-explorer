#include <QPainterPath>
#include <QPen>

#include "custom_system_model_delegate.h"
#include "../custom_system_model/custom_system_model.h"

CustomSystemModelDelegate::CustomSystemModelDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

void CustomSystemModelDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index
) const
{
	if (!index.isValid())
	{
		return;
	}

	const bool isInProgress = index.data(CustomSystemModel::CustomRoles::InProgressRole).toBool();
	const bool hasSize = !index.data(Qt::DisplayRole).toString().isEmpty();

	// size already calculated and not in progress => use parent paint.
	if (hasSize && !isInProgress)
	{
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}

	ButtonState state
	{
		static_cast<bool>(option.state & QStyle::State_MouseOver),
		static_cast<bool>(option.state & QStyle::State_Selected),
		isInProgress
	};

	_paintButton(painter, option, state);
}

void CustomSystemModelDelegate::_paintButton(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const ButtonState& state
) const
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);

	const QRectF rect = option.rect.adjusted(3, 3, -3, -3);

	_paintButtonBorder(painter, rect, state);
	_paintButtonText(painter, option.rect, state.isInProgress);

	painter->restore();
}

void CustomSystemModelDelegate::_paintButtonBorder(
	QPainter* painter,
	const QRectF& rect,
	const ButtonState& state
) const
{
	QPainterPath path;
	path.addRoundedRect(rect, 5, 5);

	QColor borderColor;
	qreal borderWidth;

	if (state.isSelected)
	{
		borderColor = kBorderPressed;
		borderWidth = 2.0;
	}
	else if (state.isHovered)
	{
		borderColor = kBorderHover;
		borderWidth = 1;
	}
	else
	{
		borderColor = kBorderDefault;
		borderWidth = 1;
	}

	QPen pen(borderColor, borderWidth);
	painter->setPen(pen);
	painter->drawPath(path);
}

void CustomSystemModelDelegate::_paintButtonText(
	QPainter* painter,
	const QRect& rect,
	bool isInProgress
) const
{
	painter->setPen(Qt::black);
	painter->drawText(
		rect,
		Qt::AlignCenter,
		isInProgress
			? QStringLiteral("Processing")
			: QStringLiteral("Calculate")
	);
}
