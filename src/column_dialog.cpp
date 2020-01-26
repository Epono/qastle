#include "column_dialog.h"
#include <QPushButton>

ColumnDialog::ColumnDialog(QWidget* parent, const int selectedColumnIndex) : QDialog(parent) {

    setupUi(this);

    this->selectedColumnIndex = selectedColumnIndex;
    this->setModal(true);

    this->comboBox->addItem("Float", QVariant((float)QMetaType::Type::Float));
    this->comboBox->addItem("Bool", QVariant((float)QMetaType::Type::Bool));
    this->comboBox->addItem("Int", QVariant((float)QMetaType::Type::Int));
    this->comboBox->addItem("String", QVariant((float)QMetaType::Type::QString));
    this->comboBox->addItem("Double", QVariant((float)QMetaType::Type::Double));

    QPushButton* okButton = this->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
    okButton->disconnect();

    connect(okButton, &QPushButton::clicked, this, &ColumnDialog::acceptedWithData);
}

void ColumnDialog::acceptedWithData() {
    this->setResult(QDialog::Accepted);
    emit accepted();
    emit finished(this->result());
    emit finishedWithData(selectedColumnIndex, this->comboBox->currentData(), this->lineEdit->text());
    hide();
}