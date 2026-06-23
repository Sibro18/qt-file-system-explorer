#include <QDir>
#include <QThread>
#include <QGuiApplication>
#include <QScreen>

#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  _layout{new QVBoxLayout},
	  _treeView{new QTreeView},
	  _lineEdit{new QLineEdit},
	  _thPool{new QThreadPool(this)},
	  _proxyModel{new CustomSortFilterProxyModel(this)},
	  _delegate{new CustomSystemModelDelegate(this)},
	  _fsModel{new CustomSystemModel(_thPool, this)},
	  _rootPath{QDir::homePath()}
{
	int maxThreadCount = QThread::idealThreadCount();

	if (maxThreadCount < 2)
	{
		maxThreadCount = 2;
	}

	_thPool->setMaxThreadCount(
		qMax(1, static_cast<int>(maxThreadCount * kThreadPoolUsageRatio))
	);

	_fsModel->setFilter(QDir::NoSymLinks | QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
	_fsModel->setRootPath(_rootPath);

	_proxyModel->setSourceModel(_fsModel);
	_proxyModel->setFilterKeyColumn(0);
	_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	_proxyModel->setRecursiveFilteringEnabled(true);

	_setupUi();

	connect(
		_treeView,
		&QTreeView::clicked,
		this,
		&MainWindow::onTreeViewItemClicked
	);
	connect(
		_lineEdit,
		&QLineEdit::textChanged,
		this,
		&MainWindow::onLineEditTextChanged
	);
}

void MainWindow::_setupUi()
{
	auto* widget = new QWidget(this);

	_layout->addWidget(_lineEdit);
	_layout->addWidget(_treeView);
	widget->setLayout(_layout);

	_treeView->setModel(_proxyModel);
	_treeView->setItemDelegateForColumn(1, _delegate);
	_treeView->setAnimated(false);
	_treeView->setIndentation(kTreeViewIndentation);
	_treeView->setSortingEnabled(true);
	_resetRootIndex();


	setCentralWidget(widget);
	setWindowTitle(kWindowTitle);

	// Centering the window.
	int x = 0, y = 0;

	if (QScreen* screen = QGuiApplication::primaryScreen())
	{
		const QRect screenGeometry = screen->availableGeometry();
		x = (screenGeometry.width() - kDefaultWindowWidth) / 2;
		y = (screenGeometry.height() - kDefaultWindowHeight) / 2;
	}

	setGeometry(x, y, kDefaultWindowWidth, kDefaultWindowHeight);
}

void MainWindow::onLineEditTextChanged(const QString& text)
{
	_proxyModel->setFilterWildcard(
		QString("*%1*").arg(text)
	);
	_resetRootIndex();
}

void MainWindow::onTreeViewItemClicked(const QModelIndex& index)
{
	if (!index.isValid() || index.column() != 1)
	{
		return;
	}

	if (index.data(Qt::DisplayRole).toString().isEmpty())
	{
		_fsModel->calculateAndSetDirSize(_proxyModel->mapToSource(index));
	}
}

void MainWindow::_resetRootIndex()
{
	_treeView->setRootIndex(
		_proxyModel->mapFromSource(
			_fsModel->index(_rootPath)
		)
	);
}
