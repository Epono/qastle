#include "../include/column_dialog.h"
#include <QPushButton>

ColumnDialog::ColumnDialog(QWidget* parent, const int selectedColumnIndex) : QDialog(parent) {

	setupUi(this);

	this->selectedColumnIndex = selectedColumnIndex;
	setModal(true);
	lineEdit->setFocus();

	comboBox->addItem("Float", QVariant((float)QMetaType::Type::Float));
	comboBox->addItem("Bool", QVariant((float)QMetaType::Type::Bool));
	comboBox->addItem("Int", QVariant((float)QMetaType::Type::Int));
	comboBox->addItem("String", QVariant((float)QMetaType::Type::QString));
	comboBox->addItem("Double", QVariant((float)QMetaType::Type::Double));

	QPushButton* okButton = buttonBox->button(QDialogButtonBox::StandardButton::Ok);
	okButton->disconnect();

	connect(okButton, &QPushButton::clicked, this, &ColumnDialog::acceptedWithData);
}

void ColumnDialog::acceptedWithData() {
	setResult(QDialog::Accepted);
	emit accepted();
	emit finished(this->result());
	emit finishedWithData(selectedColumnIndex, comboBox->currentData(), lineEdit->text());
	hide();
}