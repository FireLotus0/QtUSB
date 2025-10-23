#pragma once

#include "src/descriptor/descriptorbase/descriptorbase.h"
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN
class EndPointDesc : public DescriptorBase
{
public:
    EndPointDesc(libusb_device* device, libusb_endpoint_descriptor desc, InterfaceData* interfaceData);

    void resolveInfo() override;

private:
    TransferType parseTransferType(int type) const;

private:
    libusb_endpoint_descriptor desc;
    InterfaceData* interfaceData;
};

QT_USB_NAMESPACE_END

