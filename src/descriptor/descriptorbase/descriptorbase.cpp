#include "descriptorbase.h"
#include <qtextstream.h>
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

DescriptorBase::DescriptorBase(libusb_device *device)
    : device(device)
{
}

DescriptorBase::~DescriptorBase() {
    releaseChildren();
}

void DescriptorBase::printInfo() const {
    qCInfo(usbCategory).noquote() << getContent();
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
