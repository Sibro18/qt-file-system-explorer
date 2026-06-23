#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QThreadPool>
#include <QModelIndex>

#include "../custom_system_model/custom_system_model.h"
#include "../custom_sort_filter_proxy_model/custom_sort_filter_proxy_model.h"
#include "../custom_system_model_delegate/custom_system_model_delegate.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
private slots:
	void onTreeViewItemClicked(const QModelIndex& index);
	void onLineEditTextChanged(const QString& text);
private:
	QVBoxLayout* _layout;
	QTreeView* _treeView;
	QLineEdit* _lineEdit;
	QThreadPool* _thPool;
	CustomSortFilterProxyModel* _proxyModel;
	CustomSystemModelDelegate* _delegate;
	CustomSystemModel* _fsModel;
	const QString _rootPath;

	void _setupUi();
	void _resetRootIndex();

	static constexpr int kDefaultWindowWidth = 800;
	static constexpr int kDefaultWindowHeight = 400;
	static constexpr int kTreeViewIndentation = 20;
	static constexpr double kThreadPoolUsageRatio = 0.5;
	static constexpr const char* kWindowTitle = "Astra test task";
};

#endif // MAIN_WINDOW_H
