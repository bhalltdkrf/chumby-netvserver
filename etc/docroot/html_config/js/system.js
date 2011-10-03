// -------------------------------------------------------------------------------------------------
//	system API functions
// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------
//	server reset - browser start, and sconnected to NeTVServer
// -------------------------------------------------------------------------------------------------
function fServerReset()
{
	fDbg2("fServerReset()");
}

// -------------------------------------------------------------------------------------------------
//	press button on D-pad / android
// -------------------------------------------------------------------------------------------------
function fButtonPress( vButtonName )
{
	main_onRemoteControl(vButtonName);
}

// -------------------------------------------------------------------------------------------------
//	events from HDMI/FPGA
// -------------------------------------------------------------------------------------------------
function fHDMIEvents( vEventName )
{
	//To be decided
	if (vEventName == "unsupported");
	if (vEventName == "attach");
	if (vEventName == "detach");
	if (vEventName == "trigger");
}

// -------------------------------------------------------------------------------------------------
//	events from DBus/NetworkManager
// -------------------------------------------------------------------------------------------------
function fNMStateChanged( vEventName )
{
	configuring_onNetworkManagerState(vEventName);
	/*
	//not performing activation, don't care
	if (activationState == '')
		return;

	switch (vEventName)
	{
		case "unknown":			break;
		case "sleeping":		break;
		case "connecting":		break;
		case "disconnected":	break;
		case "connected":		break;
	}
	*/
}

function fNMDeviceAdded(  )
{
	//Switching back FROM Access Point mode
	//not performing activation, don't care
	if (activationState == '')
		return;
}

function fNMDeviceRemoved(  )
{
	//Switching TO Access Point mode
	//not performing activation, don't care
	if (activationState == '')
		return;
}

// -------------------------------------------------------------------------------------------------
//	NeTVBrowser will check this function every 60 seconds and reload the page if the reply is not 'true'
// -------------------------------------------------------------------------------------------------
function fCheckAlive()
{
	return true;
}