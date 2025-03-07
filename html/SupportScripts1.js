//==============================================================================
// Javascript for Netburner Support
// DESCRIPTION: This file shows how to use
// AJAX to call back to the netburner to update information on the page without
// doing a page refresh. 
//
// Copyright 2006 Tod Gentille Syncor Systems,Inc.
//==============================================================================


//==============================================================================
// Executable code that runs when the page that includes this file loads.
//==============================================================================
// create the AJAX http request object with global scope.
var requestObj = CreateRequestObject();
//Add the doLoad function as the window.load event keeping any 
//other event that might already be there - not necessary since we know
//there isn't another one but I want to keep it as an example for the
//technique.
addEvent(window, 'load', doLoad);


//==============================================================================
// General purpose way to add events to objects without overwriting any
// existing events that might be on that object.
//==============================================================================
function addEvent(obj, eventType, functionToRun){
 if (obj.addEventListener){
   obj.addEventListener(eventType, functionToRun, true);
   return true;
 } else if (obj.attachEvent){
   var r = obj.attachEvent("on"+eventType, functionToRun);
   return r;
 } else {
   return false;
 }
}


//==============================================================================
// When the form loads set up all the GUI elements to reflect the current
// state of the netburner
//==============================================================================
function doLoad()
{
   StartClock();
}


//==============================================================================
//******************************************************************************
// AJAX Code
//******************************************************************************
//==============================================================================

function CreateRequestObject()
{
	var request_obj;
	if(window.XMLHttpRequest)
	{
		// pretty much all browswer except IE support this
		request_obj = new XMLHttpRequest();
	} else if(window.ActiveXObject)
	{
		// Internet Explorer 5+
		request_obj = new ActiveXObject("Microsoft.XMLHTTP");
	}
	else if (window.createRequest)
	{
		request_obj = window.createRequest();
	}
	else
	{
		// There is an error creating the object,
		// just as an old browser is being used.
		alert('Problem creating the XMLHttpRequest object. Dynamic udpating will not work with this browser.');
 	}
	return request_obj;
}



//==============================================================================
// Update Values uses http.open to callback to the netburner via the callback.htm
// page which just contains a single line of html with the netburner function
// we want to call
//==============================================================================
function CallbackNetburner()
{
	requestObj.open('get','ajax.htm'); //invoke the desired function on the netburner
										//by sending the htm page that has the <!--FUNCTIONCALL AjaxCallback -->
										//The netburner method AjaxCallback() is in webformcode.cpp
										//Tell the ProcessAjaxResponse method to handle the returned data
	requestObj.onreadystatechange = ProcessAjaxResponse;
	requestObj.send(null);
}


//==============================================================================
// When the netburner writes data out the socket this method will get invoked
// when it is finished so we can retrieve the data.
//==============================================================================
function ProcessAjaxResponse()
{
	var REQUEST_DONE = 4;
	var STATUS_NORMAL = 200;
	
	if((requestObj.readyState == REQUEST_DONE)&& (requestObj.status == STATUS_NORMAL))
	{
		var answer = requestObj.responseText;
		//ajaxText is the id of a DIV tag in index.htm
		document.getElementById('ajaxTest').innerHTML = answer;
	}
	else
	{
		document.getElementById('ajaxTest').innerHTML = "Error in ProcessAjaxResponse";
	}
}


//==============================================================================
// Set up a timer so that the callback to the netburner happens every 5 seconds.
//==============================================================================
var RESET_TIMER_VALUE = 5;
var timerSecs_ = RESET_TIMER_VALUE;
function StartClock()
{
	--timerSecs_;
	setTimeout("StartClock()",1000); // delay one second
	if(timerSecs_==0)
	{
		timerSecs_ = RESET_TIMER_VALUE;
		CallbackNetburner();
	}
}

//==============================================================================
//******************************************************************************
// End AJAX Code
//******************************************************************************
//==============================================================================
