#include "descriptorbase.h"

#include <qtextstream.h>

QT_USB_NAMESPACE_BEGIN
DescriptorBase::DescriptorBase(libusb_device *device)
    : device(device)
{
}

DescriptorBase::~DescriptorBase() {
    releaseChildren();
}

void DescriptorBase::printInfo() const {
    qInfo().noquote() << getContent();
}

void DescriptorBase::releaseChildren() {
    for(auto child : children) {
        delete child;
    }
    children.clear();
}

QString DescriptorBase::getContent() const {
    QString res = content;
    for(auto child : children) {
        res += child->getContent();
    }
    return res;
}


QT_USB_NAMESPACE_END
