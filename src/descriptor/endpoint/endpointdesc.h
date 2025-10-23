#pragma once

#include "src/descriptor/descriptorbase/descriptorbase.h"
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN
class EndPointDesc : public DescriptorBase
{
public:
    EndPointDesc(libusb_device* device, libusb_endpoint_descriptor desc);

    void resolveInfo() override;

private:
    QString parseTransferType(int type) const;

private:
    libusb_endpoint_descriptor desc;
};

QT_USB_NAMESPACE_END

