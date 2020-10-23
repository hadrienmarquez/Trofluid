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
#include "ProfileCallback.h"

ProfileCallback::ProfileCallback(QObject* owner) : 
	m_owner(owner), 
	m_refCount(1)
{
}

/// IUnknown methods

HRESULT ProfileCallback::QueryInterface(REFIID iid, LPVOID *ppv)
{
	CFUUIDBytes		iunknown;
	HRESULT			result = E_NOINTERFACE;

	if (ppv == nullptr)
		return E_INVALIDARG;

	// Initialise the return result
	*ppv = nullptr;

	// Obtain the IUnknown interface and compare it the provided REFIID
	iunknown = CFUUIDGetUUIDBytes(IUnknownUUID);
	if (memcmp(&iid, &iunknown, sizeof(REFIID)) == 0)
	{
		*ppv = this;
		AddRef();
		result = S_OK;
	}

	return result;
}

ULONG ProfileCallback::AddRef(void)
{
	return ++m_refCount;
}

ULONG ProfileCallback::Release(void)
{
	ULONG newRefValue = --m_refCount;
	if (newRefValue == 0)
		delete this;

	return newRefValue;
}

/// IDeckLinkProfileCallback methods

HRESULT ProfileCallback::ProfileChanging(IDeckLinkProfile* /* profileToBeActivated */, bool streamsWillBeForcedToStop)
{
	// When streamsWillBeForcedToStop is true, the profile to be activated is incompatible with the current
	// profile and capture will be stopped by the DeckLink driver. It is better to notify the
	// controller to gracefully stop capture, so that the UI is set to a known state.
	if ((streamsWillBeForcedToStop) && (m_profileChangingCallback != nullptr))
		m_profileChangingCallback();

	return S_OK;
}

HRESULT ProfileCallback::ProfileActivated(IDeckLinkProfile *activatedProfile)
{
	// New profile activated, inform owner to update popup menus
	QCoreApplication::postEvent(m_owner, new ProfileActivatedEvent(activatedProfile));

	return S_OK;
}
