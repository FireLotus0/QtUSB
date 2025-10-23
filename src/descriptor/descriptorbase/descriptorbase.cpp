#include "descriptorbase.h"

#include <qtextstream.h>

QT_USB_NAMESPACE_BEGIN
DescriptorBase::DescriptorBase(libusb_device *device)
    : device(device)
{
}

DescriptorBase::~DescriptorBase() {
    for(auto child : children) {
        delete child;
    }
}

void DescriptorBase::printInfo() const {
    qInfo().noquote() << getContent();
}

void DescriptorBase::resolveInfo() {
}

QString DescriptorBase::getContent() const {
//    return content;
    QString res = content;
    for(auto child : children) {
        res += child->getContent();
    }
    return res;
}


QT_USB_NAMESPACE_END
