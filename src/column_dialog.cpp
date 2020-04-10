#include "../include/column_dialog.h"
#include "../include/utils.h"
#include <QPushButton>

ColumnDialog::ColumnDialog(QWidget* parent, const int selectedColumnIndex) : QDialog(parent) {

	setupUi(this);

	this->selectedColumnIndex = selectedColumnIndex;
	setModal(true);
	lineEdit->setFocus();

	comboBox->addItem("Float", QVariant((int)QastleType::TFloat));
	comboBox->addItem("Bool", QVariant((int)QastleType::TBool));
	comboBox->addItem("Int", QVariant((int)QastleType::TInt));
	comboBox->addItem("String", QVariant((int)QastleType::TString));

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