#ifndef CUSTOM_SYSTEM_MODEL_DELEGATE_H
#define CUSTOM_SYSTEM_MODEL_DELEGATE_H

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QPainter>
#include <QColor>

class CustomSystemModelDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit CustomSystemModelDelegate(QObject* parent = nullptr);
	void paint(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;
private:
	struct ButtonState
	{
		bool isHovered;
		bool isSelected;
		bool isInProgress;
	};

	void _paintButton(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		const ButtonState& state) const;
	void _paintButtonBorder(
		QPainter* painter,
		const QRectF& rect,
		const ButtonState& state
	) const;
	void _paintButtonText(
		QPainter* painter,
		const QRect& rect,
		bool isInProgress
	) const;

	static constexpr QColor kBorderDefault{180, 180, 180, 120};
	static constexpr QColor kBorderHover{80, 160, 230, 180};
	static constexpr QColor kBorderPressed{60, 140, 210, 220};
};

#endif // CUSTOM_SYSTEM_MODEL_DELEGATE_H
