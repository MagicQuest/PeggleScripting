#include "WICHelper.h"
#include "goodmacrosfordirect2dandwichelper.h"

bool WICHelper::Init() {
	SusIfFailed(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory), "yeah we failed to create the wic factory (for loading bitmaps/pictures)");
	return true;
}

IWICFormatConverter* WICHelper::LoadBitmapFromFrame(IWICBitmapDecoder* wicDecoder, IWICBitmapFrameDecode* wicFrame, GUID format, bool release) {
    IWICFormatConverter* wicConverter = NULL;
    HRESULT shit = this->wicFactory->CreateFormatConverter(&wicConverter);

    if (shit != S_OK) {
        //MessageBoxA(NULL, "Wic converter", "failed wicFactory->CreateFormatConverter(&wicConverter)", MB_OK | MB_ICONERROR);
        ERRORMB(shit, "failed wicFactory->CreateFormatConverter(&wicConverter)");
        wicDecoder->Release();
        wicFrame->Release();
        return nullptr;
    }

    shit = wicConverter->Initialize(wicFrame, format, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);
    if (shit != S_OK) {
        //MessageBoxA(NULL, "WIC CONVERTER2", "failed wicConverter->Initialize", MB_OK | MB_ICONERROR);
        ERRORMB(shit, "failed wicConverter->Initialize");
        wicDecoder->Release();
        wicFrame->Release();
        wicConverter->Release();
        return nullptr;
    }

    if (release) {
        wicDecoder->Release();
    }
    wicFrame->Release();

    return wicConverter;
}

IWICFormatConverter* WICHelper::LoadBitmapFromFilename(const wchar_t* filenamews, GUID format, int frame = 0) {
    IWICBitmapDecoder* wicDecoder = NULL;

    HRESULT shit = this->wicFactory->CreateDecoderFromFilename(filenamews/*.c_str()*/, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
    if (shit != S_OK) {
        ERRORMB(shit, "yeah we failed that hoe (CreateDecoderFromFilename)");
        //MessageBoxA(NULL, "NewWICBitmap likely failed because the file was not found", "yeah we failed that hoe (CreateDecoderFromFilename)", MB_OK | MB_ICONERROR);
        return nullptr;
    }

    IWICBitmapFrameDecode* wicFrame = NULL;
    shit = wicDecoder->GetFrame(frame, &wicFrame);

    if (shit != S_OK) {
        //MessageBoxA(NULL, "GetFirstFrameWiC", "yeah we failed the hoe (wicDecoder->GetFrame(0, &wicFrame))", MB_OK | MB_ICONERROR);
        ERRORMB(shit, "yeah we failed the hoe (wicDecoder->GetFrame(frame, &wicFrame))");
        wicDecoder->Release();
        return nullptr;
    }

    return this->LoadBitmapFromFrame(wicDecoder, wicFrame, format, true);
}

//shit CreateDecoderFromStream doesn't work the way i thought it would :( (WAIT NOW HOLD ON CHATGPT MIGHT HAVE JUST PUT ME ON)
IWICFormatConverter* WICHelper::LoadBitmapFromBinaryData(std::vector<BYTE>& stream, GUID format, GUID container, int frame = 0) {
    IWICBitmapDecoder* wicDecoder = NULL;

    //HRESULT shit = this->wicFactory->CreateDecoderFromStream((IStream*) & stringstream, NULL, WICDecodeMetadataCacheOnLoad, &wicDecoder); //oh nah it don't work like that (https://learn.microsoft.com/en-us/windows/win32/wic/-wic-decoder-howto-createusingstream)
    IWICStream *pIWICStream = NULL;
    HRESULT shit = this->wicFactory->CreateStream(&pIWICStream);
    if (shit != S_OK) {
        ERRORMB(shit, "CreateStream (LoadBitmapFromBinaryData)");
        return nullptr;
    }
    
    pIWICStream->InitializeFromMemory(stream.data(), stream.size()); //BYTE*
    //pIWICStream->InitializeFromIStream((IStream*) & stringstream);
    shit = this->wicFactory->CreateDecoderFromStream(pIWICStream, &container, WICDecodeMetadataCacheOnLoad, &wicDecoder); //oh nah it don't work like that (https://learn.microsoft.com/en-us/windows/win32/wic/-wic-decoder-howto-createusingstream)

    if (shit != S_OK) {
        ERRORMB(shit, "yeah we failed that hoe (CreateDecoderFromStream)");
        //MessageBoxA(NULL, "NewWICBitmap likely failed because the file was not found", "yeah we failed that hoe (CreateDecoderFromFilename)", MB_OK | MB_ICONERROR);
        return nullptr;
    }

    IWICBitmapFrameDecode* wicFrame = NULL;
    shit = wicDecoder->GetFrame(frame, &wicFrame);

    if (shit != S_OK) {
        //MessageBoxA(NULL, "GetFirstFrameWiC", "yeah we failed the hoe (wicDecoder->GetFrame(0, &wicFrame))", MB_OK | MB_ICONERROR);
        ERRORMB(shit, "yeah we failed the hoe (wicDecoder->GetFrame(frame, &wicFrame))");
        wicDecoder->Release();
        return nullptr;
    }

    return this->LoadBitmapFromFrame(wicDecoder, wicFrame, format, true);
}