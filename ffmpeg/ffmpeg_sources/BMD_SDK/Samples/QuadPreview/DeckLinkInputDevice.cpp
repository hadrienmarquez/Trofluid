/* -LICENSE-START-
** Copyright (c) 2019 Blackmagic Design
**
** Permission is hereby granted, free of charge, to any person or organization
** obtaining a copy of the software and accompanying documentation covered by
** this license (the "Software") to use, reproduce, display, distribute,
** execute, and transmit the Software, and to prepare derivative works of the
** Software, and to permit third-parties to whom the Software is furnished to
** do so, all subject to the following:
** 
** The copyright notices in the Software and this entire statement, including
** the above license grant, this restriction and the following disclaimer,
** must be included in all copies of the Software, in whole or in part, and
** all derivative works of the Software, unless such copies or derivative
** works are solely in the form of machine-executable object code generated by
** a source language processor.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
** -LICENSE-END-
*/

#include <QCoreApplication>
#include <QMessageBox>
#include <QTextStream>

#include "platform.h"
#include "DeckLinkInputDevice.h"

DeckLinkInputDevice::DeckLinkInputDevice(QObject* owner, com_ptr<IDeckLink>& device) :
	m_refCount(1),
	m_owner(owner),
	m_deckLink(device),
	m_deckLinkInput(IID_IDeckLinkInput, device),
	m_deckLinkConfig(IID_IDeckLinkConfiguration, device),
	m_supportsFormatDetection(false),
	m_currentlyCapturing(false),
	m_applyDetectedInputMode(false),
	m_lastValidFrameStatus(false),
	m_supportedInputConnections(bmdVideoConnectionUnspecified),
	m_selectedInputConnection(bmdVideoConnectionUnspecified)
{
}

DeckLinkInputDevice::~DeckLinkInputDevice()
{
	if (m_currentlyCapturing)
		stopCapture();
}

// IUnknown methods

HRESULT	DeckLinkInputDevice::QueryInterface(REFIID iid, LPVOID *ppv)
{
	HRESULT result = S_OK;

	if (ppv == nullptr)
		return E_INVALIDARG;

	// Obtain the IUnknown interface and compare it the provided REFIID
	if (iid == IID_IUnknown)
	{
		*ppv = this;
		AddRef();
	}
	else if (iid == IID_IDeckLinkInputCallback)
	{
		*ppv = (IDeckLinkInputCallback*)this;
		AddRef();
	}
	else
	{
		*ppv = nullptr;
		result = E_NOINTERFACE;
	}

	return result;
}

ULONG DeckLinkInputDevice::AddRef(void)
{
	return ++m_refCount;
}

ULONG DeckLinkInputDevice::Release(void)
{
	ULONG newRefValue = --m_refCount;
	
	if (newRefValue == 0)
		delete this;

	return newRefValue;
}

// IDeckLinkInputCallback methods

HRESULT DeckLinkInputDevice::VideoInputFrameArrived(IDeckLinkVideoInputFrame* /* videoFrame */, IDeckLinkAudioInputPacket* /* audioPacket */)
{
	// Since this application only previews, everything is driven from IDeckLinkScreenPreviewCallback::DrawFrame
	return S_OK;
}

HRESULT DeckLinkInputDevice::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode *newMode, BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{
	HRESULT 		result;
	BMDPixelFormat	pixelFormat;
	BMDDisplayMode	displayMode = newMode->GetDisplayMode();

	// Unexpected callback when auto-detect mode not enabled
	if (!m_applyDetectedInputMode)
		return E_FAIL;

	if (detectedSignalFlags & bmdDetectedVideoInputRGB444)
	{
		if (detectedSignalFlags & bmdDetectedVideoInput8BitDepth)
			pixelFormat = bmdFormat8BitARGB;
		else if (detectedSignalFlags & bmdDetectedVideoInput10BitDepth)
			pixelFormat = bmdFormat10BitRGB;
		else if (detectedSignalFlags & bmdDetectedVideoInput12BitDepth)
			pixelFormat = bmdFormat12BitRGB;
		else
			// Invalid color depth for RGB
			return E_INVALIDARG;
	}
	else if (detectedSignalFlags & bmdDetectedVideoInputYCbCr422)
	{
		if (detectedSignalFlags & bmdDetectedVideoInput8BitDepth)
			pixelFormat = bmdFormat8BitYUV;
		else if (detectedSignalFlags & bmdDetectedVideoInput10BitDepth)
			pixelFormat = bmdFormat10BitYUV;
		else
			// Invalid color depth for YUV
			return E_INVALIDARG;
	}
	else
		// Unexpected detected video input format flags
		return E_INVALIDARG;

	// Restart streams if either display mode or colorspace has changed
	if (notificationEvents & (bmdVideoInputDisplayModeChanged | bmdVideoInputColorspaceChanged))
	{
		// Stop the capture
		m_deckLinkInput->StopStreams();

		// Set the video input mode
		result = m_deckLinkInput->EnableVideoInput(displayMode, pixelFormat, bmdVideoInputEnableFormatDetection);

		if (result == S_OK)
			// Start the capture
			result = m_deckLinkInput->StartStreams();

		if (result != S_OK)
			// Let owner know we couldn't restart capture with detected input video mode
			QCoreApplication::postEvent(m_owner, new QEvent(kErrorRestartingCaptureEvent));
		else
			QCoreApplication::postEvent(m_owner, new DeckLinkInputFormatChangedEvent(displayMode));
	}

	return S_OK;
}

// Other methods

bool DeckLinkInputDevice::initialize()
{
	com_ptr<IDeckLinkProfileAttributes>	deckLinkAttributes(IID_IDeckLinkProfileAttributes, m_deckLink);
	dlbool_t						attributeFlag;

	// Get input interface
	if (!m_deckLinkInput)
		// This may occur if device does not have input interface, for instance DeckLink Mini Monitor.
		return false;
		
	// Get configuration interface so we can change input connector
	// We hold onto IDeckLinkConfiguration until destructor to retain input connector setting
	if (!m_deckLinkConfig)
		return false;

	// Get attributes interface
	if (!deckLinkAttributes)
		return false;

	// Check if input mode detection is supported.
	if (deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &attributeFlag) == S_OK)
		m_supportsFormatDetection = attributeFlag;
	else
		m_supportsFormatDetection = false;

	// Get the supported input connections for the device
	if (deckLinkAttributes->GetInt(BMDDeckLinkVideoInputConnections, &m_supportedInputConnections) != S_OK)
		m_supportedInputConnections = 0;

	return true;
}

bool DeckLinkInputDevice::startCapture(BMDDisplayMode displayMode, IDeckLinkScreenPreviewCallback* screenPreviewCallback, bool applyDetectedInputMode)
{
	BMDVideoInputFlags	videoInputFlags = bmdVideoInputFlagDefault;

	m_applyDetectedInputMode = applyDetectedInputMode;

	// Enable input video mode detection if the device supports it
	if (m_supportsFormatDetection && m_applyDetectedInputMode)
		videoInputFlags |=  bmdVideoInputEnableFormatDetection;

	// Set the screen preview
	m_deckLinkInput->SetScreenPreviewCallback(screenPreviewCallback);

	// Set capture callback
	m_deckLinkInput->SetCallback(this);

	// Set the video input mode
	if (m_deckLinkInput->EnableVideoInput(displayMode, bmdFormat10BitYUV, videoInputFlags) != S_OK)
		return false;

	// Start the capture
	if (m_deckLinkInput->StartStreams() != S_OK)
		return false;

	m_currentlyCapturing = true;

	return true;
}

void DeckLinkInputDevice::stopCapture()
{
	if (m_deckLinkInput)
	{
		// Stop the capture
		m_deckLinkInput->StopStreams();
		m_deckLinkInput->DisableVideoInput();

		// Delete the callbacks
		m_deckLinkInput->SetScreenPreviewCallback(nullptr);
		m_deckLinkInput->SetCallback(nullptr);
	}

	m_currentlyCapturing = false;
}

void DeckLinkInputDevice::querySupportedVideoModes(DeckLinkDisplayModeQueryFunc func)
{
	com_ptr<IDeckLinkDisplayModeIterator>	displayModeIterator;
	com_ptr<IDeckLinkDisplayMode>			displayMode;

	if (m_deckLinkInput->GetDisplayModeIterator(displayModeIterator.releaseAndGetAddressOf()) != S_OK)
		return;

	// Iterate through each supported display mode for the input connection
	while (displayModeIterator->Next(displayMode.releaseAndGetAddressOf()) == S_OK)
	{
		dlbool_t			supported = false;
		BMDDisplayMode		mode = displayMode->GetDisplayMode();

		if ((m_deckLinkInput->DoesSupportVideoMode(m_selectedInputConnection, mode, bmdFormatUnspecified, bmdNoVideoInputConversion, bmdSupportedVideoModeDefault, NULL, &supported) == S_OK)
			&& supported)
		{
			func(displayMode.get());
		}
	}
}

HRESULT DeckLinkInputDevice::setInputVideoConnection(BMDVideoConnection connection)
{
	HRESULT result = m_deckLinkConfig->SetInt(bmdDeckLinkConfigVideoInputConnection, (int64_t)connection);
	if (result != S_OK)
		return result;

	m_selectedInputConnection = connection;
	return S_OK;
}

bool DeckLinkInputDevice::isActive(void)
{
	return isDeviceActive(m_deckLink);
}

