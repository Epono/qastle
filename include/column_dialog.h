#pragma once

#include <QDialog>
#include <QWidget>
#include "ui_column.h"

// https://stackoverflow.com/questions/13116863/qt-show-modal-dialog-ui-on-menu-item-click
class ColumnDialog : public QDialog, public Ui::ColumnDialog
{
	Q_OBJECT

private:
	int selectedColumnIndex;

public:
	ColumnDialog(QWidget* parent = nullptr, const int selectedColumnIndex = -1);

public slots:
	void acceptedWithData();

signals:
	void finishedWithData(const int selectedColumnIndex, const QVariant data, const QString headerName);

};

