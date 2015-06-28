##1 

##10
| 
| ---------------------------- Start Main Frame
|
<!DOCTYPE html>
<html><head><meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />
	 
<style type='text/css'>
	a.menu { display:block;font-weight:bold; font-family: arial; font-size:15px; margin-bottom:3px; 
		       margin-top:3px;text-align:center; width:170px; border: 2px solid #AAAAAA; padding:6px; 
		       text-decoration:none;border-radius:5px; background-color:#EEF0FF;color:black;}
  
  a.menu:hover   { color:#ff0000; background-color:#FFFFFF; border: 2px solid #FFAAAA;}	 		
	
	#fade{	display: none;	position: fixed;	top: 0%%;	left: 0%%;	width: 100%%;	height: 100%%;
		      background-color: #444444;	z-index:1001;	opacity:.70;	border-radius:6px; Filter: alpha(opacity=70);}
	
	#overlay{ position:absolute;	width: 300px;	height: 500px;	top:100px; display: none;	 
	 	         font-family:arial;	 font-size:18px;	 font-weight:normal;
		         text-align:center;	 padding:0px;	 background: transparent;	 z-index:1007;}
	
	#devicemenu{ display: block;	 font-family:arial;	 font-size:17px;	 font-weight:normal;	 color:black;
		            text-align:center;	 padding:1px;	 border: 0px solid #aaaaaa;	 background: #cccccc;	 opacity: 1;
		            z-index:1006;	 border-radius: 6px; }
	
	span.submenuhead{ display:block;background-color:#bbbbbb;	padding:8px;	font-size:17px;	
		                font-weight:bold; color:#000000;	border-radius: 6px;  }
		                
	a.submenu { display:block;font-weight:normal; font-family: arial; font-size:15px;
	            margin:1px;text-align:center; padding:10px; color:black;  border: 2px solid #777777;  
	            text-decoration:none;border-radius: 6px;cursor:pointer;background-color:#eeeeee;}
	            
	a.submenu:hover   { color:#ff0000; background-color:#ffffff; border: 2px solid #FFAAAA;}	 	
</style>

<script type="text/javascript">
	var hosttxt;
  var offline;
  var maxdevices;
  var devicedata=null;

  var month= ["","Januar","Februar","März","April","Mai","Juni","Juli","August","September","Oktober","November","Dezember"];
  
  function daysInMonth(year,month) {
  	 var isLeap = new Date(year, 1, 29).getMonth() == 1;
     return [31, (isLeap ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31][month];
     }

  function getCookie(cname) {
     var name = cname + "=";
     var ca = document.cookie.split(';');
     for(var i=0; i<ca.length; i++) {
         var c = ca[i];
        while (c.charAt(0)==' ') c = c.substring(1);
         if (c.indexOf(name) == 0) return c.substring(name.length,c.length);
     }
     return "";
  } 

  function getDeviceInfo()  {
	   var Req = new XMLHttpRequest();
	   Req.onreadystatechange=function() {
	       if (Req.readyState==4 && Req.status==200) {
	        	devicedata = JSON.parse(Req.responseText);
	        	maxdevices = devicedata.devices.length;
	        	var div = document.getElementById("devicemenu");
	        	var aTag;
	        	for (a=0; a<maxdevices; a++) {
			          aTag= document.createElement('a');
			          aTag.setAttribute('href','http://'+hosttxt+'/?CMD=STATUS_DEVICE&DEV='+a);
			          aTag.setAttribute('class',"submenu");
			          aTag.setAttribute('id',"wr-details"+a);
			          aTag.setAttribute('target',"display");
			          aTag.setAttribute('onClick',"setTimeout('lclose()',150);");
			          aTag.innerHTML = devicedata.devices[a].name;
			          div.appendChild(aTag);
            }
	        }
     }
     Req.open("GET", "http://"+hosttxt+"/?CMD=GETDEVICEINFO", true);
	   Req.send(null);
   }

		function loadCanvasScript(callback)	{
		    var head = document.getElementsByTagName('head')[0];
		    var script = document.createElement('script');
		    script.type = 'text/javascript';
		    script.src = 'http://'+hosttxt+'/?CMD=CANVASJS';
		    script.onreadystatechange = callback;
		    script.onload = callback;
		    head.appendChild(script);
		}

  var CanvasLoaded = function (){
 	 	  activate(getCookie("menu"));
    }		 
		 
  function initMainform() {
  	hosttxt=window.location.hostname+":"+window.location.port;
    if (getCookie("menu") == '')
       document.cookie="menu=wr-overview;";
    loadCanvasScript(CanvasLoaded);
   	getDeviceInfo();
	  resizeaction();
  }

 function activate(link){
	 var obj= document.getElementById(link);
	 if (obj == null) {
	 	 setTimeout("activate('"+link+"');",100); 
	 	 return;
	 }
	 highlight(obj);
	 obj.click();
	 }
		 
	function lopen(){
		var rect = document.getElementById('dframe').getBoundingClientRect();
		var elem = document.getElementById('overlay').style;
		window.scrollTo(0,0);
		var l = window.innerWidth / 2 - 150;
		elem.display='block';
		elem.left = l+"px";
		document.getElementById('fade').style.display='block';	
	 }
	
	function resizeaction() {
		var l;
	  var elem = document.getElementById('overlay').style;
	  if (elem.display == 'block') {
	    l = window.innerWidth / 2 - 150;
		  elem.left = l+"px";
	    }
	}

	window.addEventListener('resize', function(event){
		resizeaction();
	});

	function lclose(){
		document.getElementById('fade').style.display='none';
	  document.getElementById('overlay').style.display='none';
	}

 function highlight( obj, val) {
   var devs= document.getElementById('dev').getElementsByTagName('a');
	 
	 for (var i=0, len = devs.length;i<len;i++){
	 if (obj.id.indexOf(devs[i].id) != -1)  
	  devs[i].style.color="red";
	 else 
	  devs[i].style.color="black";
	 }
  }		 

  function fillCanvas (canvas, color0, color1) {
      var context = canvas.getContext('2d');
      context.rect(0, 0, canvas.width, canvas.height);
      var grd = context.createLinearGradient(0, 0, 0, canvas.height);
      grd.addColorStop(0, color0);
      grd.addColorStop(1, color1);
      context.fillStyle = grd;
      context.fill();	
 }

</script></head>	

<body onloadx="initMainform();" style='width:900px;xbackground-color:#ffffff; xbackground-color:#F5F6F7; margin:0px auto; border: 0px solid red ; padding 0px;'> 

<div id="fade" onClick="lclose('');"></div>
<div id="overlay" > <!-- WR Auswahl -->
	<span class='submenuhead'>Wechselrichter Auswahl</span> 
	<div id='devicemenu' ></div>
</div>

<table style='border-radius:5px;border-color: 1px solid black;background-color:transparent;width:900px; padding:0px; margin:0px auto; margin-bottom:2px;margin-top:3px; text-align:center;' > 
<tr>
	  <td style='width:180px;padding:0px;margin:0px;border: 0px;font-weight:bold; font-family: arial; font-size:10px;'>
	      <img style='position:relative; top:5px; left:0px;margin:0px;border: 0px;' src='http://%s/?CMD=LOGO' />%s 
	  </td>
    <td style='width:120px;padding:0px;margin:0px;border: 0px solid blue;font-weight:normal; font-family: arial; font-size:13px;'>
    	<div id='id_current_ts' style='line-height:18px;padding:0px;margin:0px;'></div> 
	  </td>
    <td style='margin:0px;padding:0px;text-align:left;'>

       <table id='dev' style='background-color:transparent; margin:0px;padding 0px;height:20px;width:600px;' align='left' border='0'   cellpadding='0' cellspacing='0'><tr>
       	<td ><div align='center'><a id='wr-overview' class ='menu'  href='http://%s/?CMD=DEVICE_OVERVIEW' target='display' onclick="highlight(this,1);">Status Übersicht</a></div></td>
	     	<td ><div align='center'><a id='wr-details'  class ='menu'  href='#'  onclick="setTimeout('lopen()',25);highlight(this,1);">Detailinformationen</a></div></td>       	
       	<td ><div align='center'><a id='wr-charting' class ='menu'  href='http://%s/?CMD=LOG_ANALYSIS' target='display' onclick='highlight(this,1);'>Log-Analyse</a></div></td>
         </tr></table></td></tr></table>

<div  style='height:540px;background-color:transparent; -webkit-box-shadow: 0px 5px 42px 8px rgba(0,0,0,0.75);-moz-box-shadow: 0px 5px 42px 8px rgba(0,0,0,0.75);box-shadow: 0px 5px 42px 8px rgba(0,0,0,0.75); 
	   text-align:center; margin: 0px auto; padding:0px; border: 0px solid magenta;border-radius:6px;z-index:170'>
</div>
<iframe  style='position:absolute; top:49px;' name= 'display' id='dframe' src='' allowTransparency='true' width='900px' height='540px' scrolling='no' frameborder='0' style=' margin:0px; padding:0px;'>
</iframe>
<script type="text/javascript">
	initMainform();
</script>	
</body></html>

| ******************************************************************* WR - Details *******************************************************************

##40

<!DOCTYPE html>
<html><head><meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />

<style type='text/css'>

 #fade{
	     display: none;	position: fixed;	top: 0%%;	left: 0%%;	width: 100%%;	height: 100%%;
	     background-color: #444444;	z-index:200;	opacity:.70;	border-radius:6px;	filter: alpha(opacity=70);}
	
 table, td, tr { border: 0px solid green;	text-align:center; border-collapse:collapse; margin:0px;}
	
 table { 	width:100%%; cursor:default;	}
	
 table th {	font-family:arial;	}
 
 a.menu { display:block;font-weight:bold; font-family: arial; font-size:15px;
	  margin-bottom:3px; margin-top:3px;text-align:center; width:50px;color:black;
	  border: 1px solid red; padding:6px; text-decoration:none;border-radius:5px;cursor:pointer;background-color:#ffffff}	

 a.menupopup { display:block;font-weight:bold; font-family: arial; font-size:15px;
	  margin-bottom:0px; margin-top:0px;text-align:center; width:60px;color:#555555;
	  border: 2px solid #999999; padding:4px; text-decoration:none;border-radius:5px;cursor:pointer;background-color:#ffffff;}
	  	
 a.menupopup:hover   { color:#00f; background-color:#f5f5fd; }	  
	  
</style>	

<script type="text/javascript">

	var deviceindex=%d;
	var hosttxt=window.parent.hosttxt;
  var devicefield;
  var maxdevices = window.parent.maxdevices;
  var devicedata=window.parent.devicedata;

 function hide_info(){
 	document.getElementById('fade').style.display='none';	
 	document.getElementById('div_parameter').style.display='none';	
 	document.getElementById('div_wrstatus').style.display='none';	
 }

 function show_info (what){
 	var device
 	if (what == 0) { //parameter
 	  reload_daten("table_wrparameter1b","SCHUECO");
 	  reload_daten("table_wrparameter1a","PARAM");
 	  reload_daten("table_wrparameter2","LAENDER");
	  _target = "div_parameter";
 	  }
 	if (what == 1) { //wr-status
 		reload_daten("table_bit_betrieb","BITBETRIEB");
 		reload_daten("table_bit_stoerung","BITSTOERUNG");
 		reload_daten("table_bit_fehler","BITFEHLER");
	  _target = "div_wrstatus";
    }
 var elem= document.getElementById(_target).style;
	window.scrollTo(0,0);
	var l = window.innerWidth / 2 - 225;
	elem.display='block';
	elem.left = l+"px";

 	document.getElementById('fade').style.display='block';	
 	document.getElementById(_target).style.display='block';	
 }
 
var valuefield="0000000000";

function clearvaluefield() {
	valuefield="0000000000";
}

function setvaluefield(pos, val) {
	valuefield = valuefield.split('');
  valuefield[pos] = String.fromCharCode(48+val);
	valuefield = valuefield.join('');
}
	
//               Allgemeiner Text_Fragment Call
// ***************************************************************   

	function reload_daten(_target, _key){
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
           document.getElementById(_target).innerHTML=rq.responseText;
       }
       rq.open("GET", "http://"+hosttxt+"/?CMD=FRAG_"+_key+"&DEV="+deviceindex, true);
	     rq.send(null);
	}

//               Aenderung Leistungsreduzierung
// ***************************************************************   
	
  function set_reduction (){
  	 var Combo = document.getElementById ( 'combo_reduce' );
  	 var rq = new XMLHttpRequest();
  	 rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
         {
         	 reload_daten("table_betriebsdaten","BETRIEB");
         	 if (rq.responseText == "OK") 
              document.getElementById('id_reduce').style.color = "#00BB00";
           else
           	 document.getElementById('id_reduce').style.color = "#BB0000";           
          }
       }
     rq.open("GET", "http://"+hosttxt+"/?CMD=REDUCE&DEV="+deviceindex+"&VAL="+Combo.options[Combo.selectedIndex].value, true);
	   rq.send(null);
  }
  
  
//               Update Text Current / Yield (Ueberschriften
// ***************************************************************   

  function refresh_current() 
   {
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
         {
         	var jdata = JSON.parse(rq.responseText);
          parent.document.getElementById('id_current_ts').innerHTML=jdata.data[0].t;
          document.getElementById('id_c_current_p').innerHTML=jdata.data[0].p+" / "+Math.round(jdata.data[0].v*1000);
          document.getElementById('id_c_current_s').innerHTML=jdata.data[0].s;
         }
       }
       rq.open("GET", "http://"+hosttxt+"/?CMD=CURRENT&DEV="+deviceindex, true);
	     rq.send(null);
   }
   

//               Statusicons Wechselrichter
// ***************************************************************   

   function refresh_status() 
   {
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
         {
         	var jdata = JSON.parse(rq.responseText);
          if (jdata.data[0].offline == 1) {
			          document.getElementById('id_status_icon_1').style.display="none";
			          document.getElementById('id_status_icon_2').style.display="none";
			          document.getElementById('id_status_icon_3').style.display="none";
			          document.getElementById('id_status_offline').style.display="block";
              }
          else {    
			          document.getElementById('id_status_icon_1').style.display="block";
			          document.getElementById('id_status_icon_2').style.display="block";
			          document.getElementById('id_status_icon_3').style.display="block";
			          document.getElementById('id_status_offline').style.display="none";
			          if (jdata.data[0].betrieb == 1) 
			             document.getElementById('id_status_icon_1').style.backgroundColor="#77cc77";
			          else            
			             document.getElementById('id_status_icon_1').style.backgroundColor="#cc7777";
			                      
			          if (jdata.data[0].fehler == 1) 
			             document.getElementById('id_status_icon_2').style.backgroundColor="#77cc77";
			          else            
			             document.getElementById('id_status_icon_2').style.backgroundColor="#cc7777";
			                      
			          if (jdata.data[0].stoer == 1) 
			             document.getElementById('id_status_icon_3').style.backgroundColor="#77cc77";
			          else            
			             document.getElementById('id_status_icon_3').style.backgroundColor="#E7E101";
           }
         }
       }
       rq.open("GET", "http://"+hosttxt+"/?CMD=SMALL_STATUS&DEV="+deviceindex, true);
	     rq.send(null);
   }

//               Betriebsdaten aktualisieren
// ***************************************************************   

   function refresh_data()
   {
   	reload_daten("table_betriebsdaten","BETRIEB");	
   	
   	refresh_current();
    //updateComboReduce(); 

    refresh_status();
   	setTimeout('refresh_data();',5000);         	      
   }

//               Livechart aktualisieren
// ***************************************************************   
   
   var lasttsLivePower=0;
   
   function getLivePowerChartData() 
   {
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
	       if (rq.readyState==4 && rq.status==200) {
	        	var jdata = JSON.parse(rq.responseText);
	        	for (a=0; a<jdata.datapoints.length; a++) {
	        		if (chartLivePower.options.data[0].dataPoints.length > 50)
	        		   chartLivePower.options.data[0].dataPoints.shift();
	            chartLivePower.options.data[0].dataPoints.push({x:jdata.datapoints[a].x*1000,y:jdata.datapoints[a].y});
		          lasttsLivePower = jdata.datapoints[a].x; 
	        	}
	        	chartLivePower.render();
	        	setTimeout("getLivePowerChartData();",5000);
	       }
       }
       rq.open("GET", "http://"+hosttxt+"/?CMD=DATALIVEDATA&DEV="+deviceindex+"&TS="+lasttsLivePower, true);
	     rq.send(null);
   }

//               Stundenchart Ertrag aktualisieren
// *************************************************************** 

 function getHourChartData() 
   {
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
         {
        	var jdata = JSON.parse(rq.responseText);
          var a,b;
          for (a=0; a<jdata.devices.length+2; a++) 
        		 chartDayHour.options.data[a].dataPoints=[];
        	for (a=0; a<jdata.devices.length; a++) {
             for (b=0; b<jdata.devices[a].data.length; b++) { 
             	   if (jdata.devices[a].data[b].val>0) {
            	       chartDayHour.options.data[a].dataPoints.push({x:(jdata.devices[a].data[b].ts-3600)*1000,y:jdata.devices[a].data[b-1].val});
            	       chartDayHour.options.data[a].dataPoints.push({x:(jdata.devices[a].data[b].ts-3599)*1000,y:jdata.devices[a].data[b].val});
            	   }
         	   }
     	       chartDayHour.options.data[a].dataPoints.push({x:(jdata.devices[a].data[b-1].ts)*1000,y:jdata.devices[a].data[b-1].val});
         	}
           var sum;
           var smax = 0;
           for (b=0; b<jdata.devices[0].data.length; b++) { 
           	  sum = 0;
            	for (a=0; a<jdata.devices.length; a++) {
            		sum += jdata.devices[a].data[b].val;
            		if (sum > smax) smax = sum;
            	}
            	if (sum >jdata.devices.length) 
            	  chartDayHour.options.data[jdata.devices.length+1].dataPoints.push({x:(jdata.devices[0].data[b].ts-1800)*1000,y:sum, indexLabel:sum.toFixed(0)});
            }
        	chartDayHour.render();
        	setTimeout("getHourChartData();",5000);
         }
       }
       rq.open("GET", "http://"+hosttxt+"/?CMD=GETHOURDATA&DEV="+devicefield, true);
       rq.send(null);
     }

//               Logchart aktualisieren
// *************************************************************** 

   var lasttsDayLog=0;
   var Timeout_LogChartData;
   
  function getDayLogChartData( redo, clearts )  {
   	 clearTimeout(Timeout_LogChartData);
	   var Combo = document.getElementById ( 'combo_chart' );
	   var ctyp = Combo.options[Combo.selectedIndex].value;
	   clearvaluefield();
	   switch (ctyp) {
	   	  case "1": setvaluefield(3,1); // power ac
                  setvaluefield(0,1); // reduce
                  chartDayLog.options.axisY.labelFontColor = "#333333";
                  chartDayLog.options.axisY2.labelFontColor = "#333333";

	     		        chartDayLog.options.data[1].axisYType = "secondary";
	     		        chartDayLog.options.data[1].type = "line";
	     		        chartDayLog.options.data[1].lineDashType = "dash";

	   	            break;
	   	  case "2": setvaluefield(2,1); // power dc
                  setvaluefield(0,1); // reduce
                  chartDayLog.options.axisY.labelFontColor = "#333333";
                  chartDayLog.options.axisY2.labelFontColor = "#333333";

	     		        chartDayLog.options.data[1].axisYType = "secondary";
	     		        chartDayLog.options.data[1].type = "line";

	   	            break;
	   	  case "3": setvaluefield(2,1); // power dc
                  setvaluefield(4,1); // voltage dc
                  chartDayLog.options.axisY.labelFontColor = chartDayLog.options.data[1].color;
                  chartDayLog.options.axisY2.labelFontColor = chartDayLog.options.data[0].color;

	     		        chartDayLog.options.data[1].axisYType = "primary";
	     		        chartDayLog.options.data[1].type = "line";
	   	            break;
	   	  case "4": setvaluefield(2,1); // power dc
                  setvaluefield(5,1); // current dc
                  chartDayLog.options.axisY.labelFontColor = chartDayLog.options.data[1].color;
                  chartDayLog.options.axisY2.labelFontColor = chartDayLog.options.data[0].color;

       		        chartDayLog.options.data[1].axisYType = "primary";
       		        chartDayLog.options.data[1].type = "line";

	   	            break;
	   	  case "5": setvaluefield(3,1); // power ac
                  setvaluefield(7,1); // current ac
                  chartDayLog.options.axisY.labelFontColor = chartDayLog.options.data[1].color;
                  chartDayLog.options.axisY2.labelFontColor = chartDayLog.options.data[0].color;
       		        chartDayLog.options.data[1].axisYType = "primary";
       		        chartDayLog.options.data[1].type = "line";
	   	            break;
	   	  case "6": setvaluefield(3,1); // power ac
                  setvaluefield(6,1); // voltage dc
                  chartDayLog.options.axisY.labelFontColor = chartDayLog.options.data[1].color;
                  chartDayLog.options.axisY2.labelFontColor = chartDayLog.options.data[0].color;
       		        chartDayLog.options.data[1].axisYType = "primary";
       		        chartDayLog.options.data[1].type = "line";

	   	            break;

	     }
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function() {
       	if (rq.readyState==4 && rq.status==200)
         {
        	jdata = JSON.parse(rq.responseText);
          parent.document.getElementById('id_current_ts').innerHTML=jdata.time;
           
           var ce = chartDayLog.options.data;
				   if (clearts) {
				      ce[0].dataPoints = [];
				      ce[1].dataPoints = [];
				      ce[2].dataPoints = [];
				      }

             for (b=0; b<jdata.devices[0].data.length; b++) { 
             	 var je = jdata.devices[0].data[b];
						   switch (ctyp) {
						   	  case "1":   ce[0].dataPoints.push({x:je.ts*1000,y:je.pac});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.red});
						   	          break;
						   	  case "2":   ce[0].dataPoints.push({x:je.ts*1000,y:je.pdc});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.red});
						   	          break;
						   	  case "3":   ce[0].dataPoints.push({x:je.ts*1000,y:je.pdc});
  														ce[1].dataPoints.push({x:je.ts*1000,y:je.vdc});
						   	          break;						   	          
						   	  case "4":   ce[0].dataPoints.push({x:je.ts*1000,y:je.pdc});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.cdc});
						   	          break;						   	          
						   	  case "5":   ce[0].dataPoints.push({x:je.ts*1000,y:je.pac});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.cac});
						   	          break;						   	          
						   	  case "6":   ce[0].dataPoints.push({x:je.ts*1000,y:je.pac});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.vac}); // effektivstrom
						   	          break;						   	          
						    }
        	       lasttsDayLog = je.ts;
       	    
        	  }
        	
        	chartDayLog.render();

        	if (redo == 1) Timeout_LogChartData = setTimeout("getDayLogChartData(1,0);",5000);
         }
       }
	   if (clearts) {
	      lasttsDayLog = 0;
        document.getElementById("combo_chart").blur();
	      }
     rq.open("GET", "http://"+hosttxt+"/?CMD=GETLOGDATA&DEV="+devicefield+"&TS="+lasttsDayLog+"&VAL="+valuefield, true);
     rq.send(null);
     }

  function getTimeString ( seconds ){
    return (new Date(seconds)).toLocaleTimeString(); 
  }

 

//               CANVAS.JS Charts initialisieren
// ***************************************************************     
  
var chartDayHour;    
var chartLivePower;   
var chartDayLog;
var chartDayYield;

function createCharts() { 

  chartDayHour = new window.parent.CanvasJS.Chart(document.getElementById("chartDayHourID"),
					    {
	            zoomEnabled:false,
				    	animationEnabled: false,
				    	backgroundColor:"transparent",
				    	margin: 0,

					    axisX:{
					    	     margin:0,
					    	     interval:1,
					        	 intervalType: "hour",
					        	 gridThickness: 1,
					           valueFormatString :"HH",
					           labelFontColor: "#333333",
					           labelFontSize: 12,
					           margin:0,
					           lineThickness:1,
					           tickThickness:1
					    			},
		    			axisY: {
				    	       margin:5,
					           lineThickness:1,
					           tickThickness:1
					            },
					    axisY2: {
					        	 gridThickness: 1,
					        	 minimum:0,
				    	       labelFontColor: "#333333",
				    	       labelFontSize: 12,
				    	       margin:0,
					           lineThickness:1,
					           tickThickness:1
					            },
					    data: [
					           {  
					           	toolTipContent:{enabled:false},
							    		markerType:"none",	    
							    	  lineThickness:0, 
							    	  type: "stackedArea",
							        fillOpacity: .6, 
							        xValueType: "dateTime",
							        axisYType:"secondary",
							        dataPoints: []
							    		},
  						    		{  // Dummy for left yaxis  
							        axisYType:"primary",
							        dataPoints: []
							    		},
												    		
                      {   
											toolTipContent:"{y} W",
				              indexLabelPlacement: "outside",  
				              indexLabelOrientation: "horizontal",
				              indexLabelFontColor: "#111111",
                      indexLabelFontSize: 11,
							    		markerType:"none",	    
							    	  color:"transparent", 
							    	  type: "line",
							        xValueType: "dateTime",
							        axisYType:"secondary",
							        dataPoints: []
							    		},													    		
                       { axisYType:"primary", dataPoints: []	}

					    			]
					  	}
  	      );

  chartLivePower = new window.parent.CanvasJS.Chart(document.getElementById("chartLivePowerID"),
					    {
				    	animationEnabled: false,
				    	backgroundColor:"transparent",
				    	margin: 10,
				    	toolTip:{             
        content: function(e){
          var content;
          content = getTimeString(e.entries[0].dataPoint.x) + "<br><strong>"+e.entries[0].dataPoint.y +" W" ;
          return content;
        }},

					    axisX:{
					    	     margin:0, gridThickness: 1,
					           valueFormatString :"HH:mm",
					           labelFontColor: "#333333",
					           labelFontSize: 12,
					           lineThickness:1, tickThickness:1
					    			},
		    			
					    axisY: {
					        	 gridThickness: 0,
				    	       margin:5,
				    	       lineThickness:1
					            },

					    axisY2: {
					    	     minimum:0,
					        	 gridThickness: 1,
				    	       labelFontColor: "#333333",
				    	       labelFontSize: 12,
				    	       margin:0,
					           lineThickness:1,
					           tickThickness:1
					            },
					    data: [
							    		{   
							    		markerType:"none",	    
							    	  lineThickness:0.5, 
							    	  color:"#0000ff",
							        type: "area",
							        fillOpacity: .3, 
							        xValueType: "dateTime",
							        axisYType:"secondary",
							        dataPoints: []
							    		},
							    		{  // Dummy for left yaxis  
							        axisYType:"primary",
							        dataPoints: []
							    		}
							    		
					    			]
					  	}
  	          );
 chartDayLog = new window.parent.CanvasJS.Chart(document.getElementById("chartDayLogID"),
					     {
					     	zoomEnabled:true,
					    	animationEnabled: false,
					    	animationDuration:100,
					    	backgroundColor:"transparent",//"#eeeeee",
					     
					    axisX:{
					    	     margin:5,
 					        	 gridThickness: 1,
					        	 labelFontSize: 12,
					        	 labelFontColor: "#333333",
					           valueFormatString :"HH:mm",
					           lineThickness:1,
					           tickThickness:1
					    			},
					    axisY2: {
					    	       title:" ",
					    	       titleFontSize:8,
					    	       margin:0,
					    	       minimum:0,
					    	       labelFontColor: "#333333",
					    	       labelFontSize: 12,
					        	   gridThickness: 1,
					    	       intervalType: "number",
					    	       interval: 50,
					             lineThickness:1,
					             tickThickness:1,
					             
					            },
					    axisY: { // Dummy for left axis
					    	 titel:"test",
					    	      margin:5,
					    	      gridThickness: 0,
					            lineThickness:1,
					            tickThickness:1
					            },

					    data: [
							    		{       
							    	  markerType:"none",
							    	  lineThickness:0, 
							    	  color:"#00aa33",
							        type: "area",
							        fillOpacity: 0.6, 
							        xValueType: "dateTime",
							        axisYType:"secondary",
							        dataPoints: []
							    		},
							    		{ 
							    		markerType:"none",      
							    	  lineThickness:1, 
							    	  color:"#aa0000",
							        type: "line",
							        xValueType: "dateTime",
							        axisYType:"secondary",
							        dataPoints: []
							    		},
							    		{ 
							    		markerType:"none",      
							    	  lineThickness:1, 
							    	  color:"#aa0000",
							        type: "line",
							        xValueType: "dateTime",
							        axisYType:"primary",
							        dataPoints: []
							    		},
					    			]
					  	}
  	         );
}
 
 function initCharts(){
 	//var ex =  document.getElementById('id_wr_text');
  //if (typeof(ex) == 'undefined' || ex == null) {
  //  setTimeout('initCharts();',100);
   // return;
   // } 
 	
   document.cookie="menu=wr-details"+deviceindex+";";
 	 document.getElementById('id_wr_text').innerHTML=devicedata.devices[deviceindex].id+" - "+devicedata.devices[deviceindex].name;
 	 devicefield = "";
   for (a=0; a<maxdevices; a++)
     if (a==deviceindex)
       devicefield+="1";
     else  
       devicefield+="0";

 	 createCharts();
 	 initComboReduce();
 	 getDayLogChartData(1,1); 
 	 getHourChartData(); 
 	 chartDayHour.render(); 
 	 getLivePowerChartData(); 
 	 chartLivePower.render(); 
   
   window.parent.fillCanvas(document.getElementById('Canvas2'),'#EEF0FF','#C1C6FF');
   window.parent.fillCanvas(document.getElementById('Canvas1'),'#EFF1FF','#EEF0FF');
     
 	 setTimeout('refresh_data();',100);
 }

 function initComboReduce() {
  	if (devicedata == null) {
  		setTimeout('initComboReduce();',100); 
  	  return;
  		}
  	var elem = document.getElementById('maxpower');
  	elem.innerHTML = "von " + devicedata.devices[deviceindex].maxpower+ " W";
  	elem = document.getElementById('combo_reduce');
  	for (a=0; a<=devicedata.devices[deviceindex].maxpower; a+=10) {
  		var opt = document.createElement("option");
  		if (a-5 < devicedata.devices[deviceindex].setpower && a+5 >= devicedata.devices[deviceindex].setpower) 
  			opt.selected=true;
  		opt.value =a;
  		opt.text=a;
  		elem.appendChild(opt);
  	}
  }
  
   function updateComboReduce( ) {
    val = devicedata.devices[deviceindex].setpower;
  	elem = document.getElementById('combo_reduce');
  	for (a=0; a<elem.options.length; a++) {
  		opt = elem[a];
  		if (opt.value-5 < val && opt.value+5 >= val) 
  			opt.selected=true;
  	}
   }
  
</script></head>

<body  onloadx="initCharts();" style='width:100%%px; margin:auto; border: 0px; padding 0px;font-family:arial;font-size:15px;'> 

| hidden elements

<div id="fade" onClick="hide_info();"></div>
 
<div id='div_parameter' onclick='hide_info();' style='position:absolute; border-radius:5px;background-color:#eeeeee;top:20px;width:500px; height:500px; border:1px solid #aaaaaa; z-index:301; right:10px; display:none; '>
	 <div style='font-family:arial; font-size:15px; text-align:center; font-weight:bold; margin:7px; '>Wechselrichter Parameter</div><hr style='position:absolute; top:25px;left:0px;width:100%%'/>		

   <div style='position:absolute; top:45px; left:5px; width:245px; height:170px; border:0px solid black;font-family:arial; font-weight:bold; font-size:13px;'>Info Wechselrichter:</div>
   <div id='div_wrparameter1a' style='position:absolute; top:65px; left:5px; width:240px; height:170px; border:0px solid black;'>
   		   	<table  id='table_wrparameter1a' style='vertical-align:text-top;margin-top:0px;	width:230px;cursor:pointer;'>
	   	</table>
   </div>


   <div style='position:absolute; top:45px; right:5px; width:245px; height:170px; border:0px solid black;font-family:arial; font-weight:bold; font-size:13px;'>Schueco/KACO:</div>
   <div id='div_wrparameter1b' style='position:absolute; top:65px; right:5px; width:240px; height:170px; border:0px solid black;'>
	   	<table  id='table_wrparameter1b' style='vertical-align:text-top;margin-top:0px;	width:230px;cursor:pointer;'>
	   	</table>
   </div>

   <div style='position:absolute; top:247px; left:5px; width:240px; height:170px; border:0px solid black;font-family:arial; font-weight:bold; font-size:13px;'>Ländereinstellungen:</div>
	 <div id='div_wrparameter2' style='position:absolute; top:267px; left:5px; width:490px; height:210px; border:0px solid black;'>
	   	<table  id='table_wrparameter2' style='vertical-align:text-top;margin-top:0px;	width:490px;cursor:pointer;'>
	   	</table>
   </div>


</div>	

<div id='div_wrstatus' onclick='hide_info();' style='position:absolute; border-radius:5px;background-color:#eeeeee;top:5px;width:500px; height:530px; border:1px solid #aaaaaa; z-index:201; right:10px; display:none; '>
	  <div style='font-family:arial; font-size:15px; text-align:center; font-weight:bold; margin:10px; '>Wechselrichter Status</div>		
   
   <div style='position:absolute; top:40px; left:5px; width:245px; height:170px; border:0px solid black;font-family:arial; font-weight:bold; font-size:13px;'>Info Wechselrichter:</div>
   <div id='div_bit_betrieb' style='position:absolute; top:60px; left:5px; width:240px; height:170px; border:0px solid black;'>
   		   	<table  id='table_bit_betrieb' style='vertical-align:text-top;margin-top:0px;	width:230px;cursor:pointer;'>
	   	</table>
   </div>
	

   <div style='position:absolute; top:40px; right:0px; width:245px; height:170px; border:0px solid black;font-family:arial; font-weight:bold; font-size:13px;'>Fehler:</div>
   <div id='div_bit_fehler' style='position:absolute; top:60px; right:5px; width:240px; height:170px; border:0px solid black;'>
	   	<table  id='table_bit_fehler' style='vertical-align:text-top;margin-top:0px;	width:230px;cursor:pointer;'>
	   	</table>
   </div>
   
   <div style='position:absolute; top:260px; left:5px; width:240px; height:170px; border:0px solid black;font-family:arial; font-weight:bold; font-size:13px;'>Störung:</div>
	 <div id='div_bit_stoerung' style='position:absolute; top:280px; left:5px; width:240px; height:210px; border:0px solid black;'>
	   	<table  id='table_bit_stoerung' style='vertical-align:text-top;margin-top:0px;	width:230px;cursor:pointer;'>
	   	</table>
   </div>
 	</div>    
|*** the visible elements

<div style='height:33px;'></div>
  <div style='position:absolute; top:8px; left:15px; font-size:15px; color:blue; font-weight:normal; text-align:left;'>
	 <a class ='menupopup' style='width:220px;color:black'  id='id_wr_text' onclick='show_info(0);'></a>
  </div>
	
	<div style='position:absolute; top:8px; left:390px;'>
		<a class ='menupopup' style='width:110px; hight:25px;text-align:left;' id='id_statusinfo'  onclick='show_info(1);'>Status
			<div id='id_status_offline' style='display:none;position:absolute; top:5px; left:55px; width:50px; height:20px; '>offline</div>
      <div id='id_status_icon_1' style='display:block;position:absolute; top:5px; left:55px; width:20px; height:20px; background-color:#aaaaaa; border-radius:3px;'></div>
      <div id='id_status_icon_2' style='display:block;position:absolute; top:5px; left:76px; width:20px; height:20px; background-color:#aaaaaa; border-radius:3px;'></div>
      <div id='id_status_icon_3' style='display:block;position:absolute; top:5px; left:97px; width:20px; height:20px; background-color:#aaaaaa; border-radius:3px;'></div>
    </a>
	</div>	

<div style='position:absolute; margin:0px;top:8px; right:13px;font-size:15px;font-family:arial;'>	
	<div style='display:inline-block;'>Leistung max.</div>
	<div style=';display:inline-block;'>	
	     <select onchange='set_reduction();' id='combo_reduce' size='1' name='rcombo' style='padding:2px;border-radius: 5px;border:2px solid #8888aa;font-size:16px;background-color:#FAFAFF;'>
	     </select>	
	</div>
	<div id='maxpower' style='display:inline-block;'></div>
</div>


<div style='position:absolute; margin:0px;top:250px; right:30px;font-size:15px;font-family:arial; z-index:150;'>	
		<form style='cursor:default; margin:0px; padding:0px;'  target=''    >
	     <select onchange='getDayLogChartData(1,1); ' id='combo_chart' size='1' style='width:230px;padding:2px;border-radius: 5px;border:2px solid #8888aa;font-size:16px;background-color:#FAFAFF;'>
	     	   <option value='1' selected>AC: Leistung & Begrenzung</option>
	     	   <option value='2'         >DC: Leistung & Begrenzung</option>
	     	   <option value='3'         >DC: Leistung & Spannung</option>
	     	   <option value='4'         >DC: Leistung & Strom</option>
	     	   <option value='5'         >AC: Leistung & Strom</option>
	     	   <option value='6'         >AC: Leistung & Spannung</option>

	     </select>	
	    
	  </form>
	
</div>



<div style='position:absolute; top:53px; left:370px;width:220px; text-align:center;' >Aktuell: <span id='id_c_current_p' style='color:red; font-size:18px;font-weight:bold;'></span> W</div>	
<div style='position:absolute; top:53px; left:45px;width:220px; text-align:center;' >Summe: <span id='id_c_current_s' style='color:red; font-size:18px;font-weight:bold;'></span> W</div>

<div style='height:190px;'></div>
	<div style='position:absolute; top:60px; right:25px;height:200px;width:220px;'>
  <table onclick='reload_daten("table_betriebsdaten","BETRIEB");' id='table_betriebsdaten' style='vertical-align:text-top;	line-height:16px;:0px;margin-top:0px;cursor:pointer;	'></table>
</div>		
		
	
// ----- Charts !!!	
<div>
<div id="chartDayLogID" style='position:absolute; top:280px; left:0px; height: 275px; width:895px;z-index:111; border:0px solid red;margin:0px;'></div>

<div id="chartLivePowerID" style='position:absolute; top:70px; left:340px; height: 230px; width:290px;z-index:100; border:0px;margin-left:0px;'></div>
|<div id="chartDayLogID" style='position:absolute; top:280px; left:0px; height: 280px; width:880px;z-index:110; border:0px solid red;margin:0px;'></div>
<div id="chartDayHourID" style='position:absolute; top:70px; left:0px; height: 230px; width:340px;z-index:100; border:0px;margin-left:0px;'></div>
</div> 

<canvas id="Canvas2" style='display:block;position:absolute; top:280px; left:0px; height: 260px; width:100%%; z-index:105; border-radius: 6px;'></canvas>
<canvas id="Canvas1" style='display:block;position:absolute; top:0px; left:0px; height: 100%%; width:100%%; z-index:-100;border-radius: 6px; '></canvas>
<script type="text/javascript">
	initCharts();
	//alert("HALLO");
</script>	
</body>


|----  Tabellenausgabe für betriebsdaten / parameter ...  ----- 
##41  
<tr style='height:35px;'><td><div style='text-align:left;font-family:arial; font-size:13px;'>%s</div></td>
	  <td>
	  <div style='float:right;text-align:center;font-size:13px;font-family:arial;font-weight:bold;width:20px; height:20px; border: 1px solid #aaaaaa;line-height:21px;background-color:#ffaaaa; border-radius:3px;'>%s</div>
	  <div style='float:right;text-align:center;font-size:13px;font-family:arial;font-weight:bold;width:20px; height:20px; border: 1px solid #aaaaaa;line-height:21px;margin-right:3px;background-color:#ffffaa; border-radius:3px;'>%s</div>
	  <div style='float:right;text-align:center;font-size:13px;font-family:arial;font-weight:bold;width:20px; height:20px; border: 1px solid #aaaaaa; line-height:21px;margin-right:3px;background-color:#aaffaa; border-radius:3px;'>%s</div>
	  </td>
</tr>

##42  
<tr><td><div style='text-align:left;font-family:arial; font-size:13px;'>%s</div></td>
	  <td><div style='text-align:right;font-size:13px;font-family:arial;'>%s</div></td>
	  <td><div style='text-align:left;font-size:13px;font-family:arial;'>%s</div></td>
	  
</tr>
##43  
<tr><td><div style='text-align:left;font-family:arial; font-size:13px;'>%s</div></td>
	  <td><div style='text-align:right;font-family:monospace, Fixed;'>%1.2f</div></td>
	  <td><div style='text-align:left;font-family:monospace, Fixed;'>%s</div></td>
</tr>
|----  Sonderfall Leistungsreduzierung mit ID für Farbe ----
##44  
<tr><td><div id='id_reduce' style='text-align:left;font-family:arial; font-size:13px;'>%s</div></td>
	  <td><div style='text-align:right;color:#008800;font-weight:bold;font-family:monospace, Fixed;'>%1.0f</div></td>
	  <td><div style='text-align:left;font-family:monospace, Fixed;'>%s</div></td>
</tr>
##45
| ----- Bitcodes  2 Spaltig  
<tr><td><div style='text-align:left;font-family:arial; font-size:12px;'>%s</div></td>
	  <td><div style='text-align:right;font-size:12px;font-family:monospace, Fixed;'>%s</div></td>
</tr>
##46  
<tr><td><div style='text-align:left;font-family:arial; font-size:13px;'>%s</div></td>
	  <td><div style='text-align:right;font-size:13px;font-family:arial;'>%s</div></td>
  
</tr>

| -------------------------------------------------------------------------------------------------------------------------------------------------------

| ******************************************************************* WR - Uebersicht *******************************************************************

| -------------------------------------------------------------------------------------------------------------------------------------------------------

##60
<!DOCTYPE html>
<html><head>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />
<script type="text/javascript">

  var numdevices=window.parent.maxdevices;

  var hosttxt=window.parent.hosttxt;
  
  var maxdevices = window.parent.maxdevices;
  var devicedata=window.parent.devicedata;
	
	function reload_daten(_target, _key){
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function() {
       	if (rq.readyState==4 && rq.status==200)
           document.getElementById(_target).innerHTML=rq.responseText;
       }
       rq.open("GET", "http://"+hosttxt+"/?CMD=FRAG_"+_key, true);
	     rq.send(null);
	}

  function refresh_current_overview() {  // Barchart
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function() {
       	if (rq.readyState==4 && rq.status==200)
         {
         	jdata = JSON.parse(rq.responseText);
         	
         	chartStatusBar.options.data[0].dataPoints = []; // current
         	chartStatusBar.options.data[1].dataPoints = []; // reduce
         	for (a=0; a<jdata.data.length; a++) { 
         		 var txt; 
         		 if (a  < maxdevices) 
         		    txt = "PV-"+a.toString();
         		 else 
         		 	  txt = "EV";   
         	   chartStatusBar.options.data[0].dataPoints.push({y:Math.round(jdata.data[a].current),  value: jdata.data[a].current.toFixed(0)});
             chartStatusBar.options.data[1].dataPoints.push({y:Math.round(jdata.data[a].reduce)-Math.round(jdata.data[a].current),  label:txt,value: jdata.data[a].current.toFixed(0)});
            }         	
         	chartStatusBar.render();
         }
       }
       rq.open("GET","http://"+hosttxt+"/?CMD=CURRENT&DEV=ALL", true);
	     rq.send(null);
   }

		function redirect_to_detail (e) {
			  id = e.dataSeriesIndex;
			if (id<100) {
				parent.document.getElementById('wr-details').style.color='red';
				parent.document.getElementById('wr-overview').style.color='black';
				location.replace("http://"+hosttxt+"/?CMD=STATUS_DEVICE&DEV="+id);
		   }
		}

var devicefield;
var valuefield="0000000000";

function clearvaluefield() {
	valuefield="0000000000";
}

function setvaluefield(pos, val) {
	valuefield = valuefield.split('');
  valuefield[pos] = String.fromCharCode(48+val);
	valuefield = valuefield.join('');
}

var chartSumDougnut;
var chartDayLogSum;
var chartDayHourSum;
function createCharts() { 

window.parent.CanvasJS.addColorSet("customColorSet1",
     [//colorSet Array
      "#DDDD00",
      "#DDCC00",
      "#DDBB00",
      "#DDAA00",
      "#DD9900"  
    ]); 

window.parent.CanvasJS.addColorSet("StackedSum",
     [ "#117711", 
       "#22AA22",
       "#33FF33",
       "#DDAA00",
       "#DD7700"  
    ]); 


	
	chartDayLogSum = new window.parent.CanvasJS.Chart(document.getElementById("chartDayLogSumID"),
					     {
					     	zoomEnabled:true,
					    	animationEnabled: false,
					    	backgroundColor: "transparent",//"#eeeeee",
					      colorSet:"StackedSum",
					    axisX:{
					    	     margin:0,
 					        	 gridThickness: 1,
					        	 labelFontSize: 12,
					        	 labelFontColor: "#333333",
					           valueFormatString :"HH:mm",
					           lineThickness:1,
					           tickThickness:1
					    			},
					    axisY: {
				    	       margin:5,
					           lineThickness:1,
					           tickThickness:1
					            },
					    axisY2: {
					    	       title: " ",
					    	       titleFontSize:8,
					    	       labelFontColor: "#333333",
					    	       labelFontSize: 12,
					        	   gridThickness: 1,
					    	       margin:0,
					             lineThickness:1,
					             tickThickness:1
					            },
					    data: [							    		
					    			]
					  	}
  	         );
  	         
 chartDayHourSum = new window.parent.CanvasJS.Chart(document.getElementById("chartDayHourSumID"),
					    {
	            zoomEnabled:true,
				    	animationEnabled: false,
				    	backgroundColor:"transparent",
				    	margin: 0,

					    axisX:{
					    	     margin:0,
					        	 gridThickness: 1,
					        	 interval:1,
					        	 intervalType: "hour",
					           valueFormatString :"HH",
					           labelFontColor: "#333333",
					           labelFontSize: 12,
					           margin:0,
					           lineThickness:1,
					           tickThickness:1
					    			},
		    			axisY: {
				    	       margin:5,
					           lineThickness:0,
					           tickThickness:0
					            },
					    axisY2: {
					    	     minimum:0,
					        	 gridThickness: 1,
				    	       labelFontColor: "#333333",
				    	       labelFontSize: 12,
				    	       margin:0,
					           lineThickness:1,
					           tickThickness:1
					            },
					    data: [
					    			]
					  	}
  	      );

chartStatusBar = new window.parent.CanvasJS.Chart(document.getElementById("chartStatusBarID"),
					    {
	            zoomEnabled:true,
				    	animationEnabled: false,
				    	backgroundColor:"transparent",
				    	margin: 0,
					    axisX:{
					    	     margin:0,
					        	 gridThickness: 0,
					           valueFormatString :"HH:mm",
					           labelFontColor: "#333333",
					           labelFontSize: 12,
					           margin:0,
					           lineThickness:1,
					           tickThickness:1
					    			},
		    			axisY: {
				    	       margin:5,
					           lineThickness:1,
					           tickThickness:1
					            },
					    axisY2: {
					        	 gridThickness: 1,
				    	       labelFontColor: "#333333",
				    	       labelFontSize: 12,
				    	       margin:0,
					           lineThickness:1,
					           tickThickness:1
					            },
					    data: [
					    			]
					  	}
  	      );

	
}

       

 var lasttsDayLogSum=0;
 
 function getDaySumChartData() 
   {
	   var rq;
	   rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
         {
        	jdata = JSON.parse(rq.responseText);
          parent.document.getElementById('id_current_ts').innerHTML=jdata.time;

        	for (a=0; a<jdata.devices.length; a++) { 
             for (b=0; b<jdata.devices[a].data.length; b++) { 
             		      var dat = jdata.devices[a].data[b].pac;
             		      if (dat < 0) dat = 0;
                	    chartDayLogSum.options.data[a].dataPoints.push({x:jdata.devices[a].data[b].ts*1000,y:dat});
        	            lasttsDayLogSum = jdata.devices[a].data[b].ts;
        	    
        	  }
        	}
        	
        	chartDayLogSum.render();

        	setTimeout("getDaySumChartData();",5000);
         }
       }
       clearvaluefield();
       setvaluefield(3,1);
       xrequest ="http://"+hosttxt+"/?CMD=GETLOGDATA&DEV="+devicefield+"&TS="+lasttsDayLogSum+"&VAL="+valuefield;

       rq.open("GET", xrequest, true);
       rq.send(null);
     }
       


 function getDayHourChartData() 
   {
	   var rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
         {
        	var jdata = JSON.parse(rq.responseText);
          var a,b;
          for (a=0; a<jdata.devices.length+2; a++) 
        		 chartDayHourSum.options.data[a].dataPoints=[]; 
        	for (a=0; a<jdata.devices.length; a++) {
             for (b=0; b<jdata.devices[a].data.length; b++) { 
             	   if (jdata.devices[a].data[b].val>0) {
            	       chartDayHourSum.options.data[a].dataPoints.push({x:(jdata.devices[a].data[b].ts-3600)*1000,y:jdata.devices[a].data[b-1].val});
            	       chartDayHourSum.options.data[a].dataPoints.push({x:(jdata.devices[a].data[b].ts-3599)*1000,y:jdata.devices[a].data[b].val});
            	       }
         	   }
     	       chartDayHourSum.options.data[a].dataPoints.push({x:(jdata.devices[a].data[b-1].ts)*1000,y:jdata.devices[a].data[b-1].val});
         	}
           var sum;
           for (b=0; b<jdata.devices[0].data.length; b++) { 
           	  sum = 0;
            	for (a=0; a<jdata.devices.length; a++) {
            		sum += jdata.devices[a].data[b].val;
            	}
            	if (sum >jdata.devices.length)
            	  chartDayHourSum.options.data[jdata.devices.length+1].dataPoints.push({x:(jdata.devices[0].data[b].ts-1800)*1000,y:sum, indexLabel:sum.toFixed(0)});
            }

        	chartDayHourSum.render();

        	setTimeout("getDayHourChartData();",5000);
         }
       }
       rq.open("GET", "http://"+hosttxt+"/?CMD=GETHOURDATA&DEV="+devicefield, true);
       rq.send(null);
     }



 function refresh_data()
   {
   	refresh_current_overview(); 
   	reload_daten("table_statistik","STATISTIK");
   	setTimeout('refresh_data();',5000);         	      
   }

 function initCharts(){
   var ex =  document.getElementById('Canvas1');
  if (typeof(ex) == 'undefined' || ex == null) {
    setTimeout('initCharts();',100);
    return;
    } 
 	 document.cookie="menu=wr-overview;";
 	 devicefield="";
 	 createCharts();

 	 chartDayHourSum.data=[];
 	 chartDayLogSum.data=[];

 	 for (a=0; a<numdevices; a++) {
 	 	 devicefield += '1';
 	   chartDayHourSum.options.data.push({  
													    		toolTipContent:{enabled:false}, 
													    		markerType:"none",	    
													    	  lineThickness:0, 
													    	  type: "stackedArea",
													        fillOpacity: .6, 
													        xValueType: "dateTime",
													        axisYType:"secondary",
													        dataPoints: []
													    		});


 	   chartDayLogSum.options.data.push({       
													    	  markerType:"none",
													    	  lineThickness:0.5, 
													    	  lineColor:"#00FF00",
													    	  type: "stackedArea",
													        fillOpacity: .7, 
													        xValueType: "dateTime",
													        axisYType:"secondary",
													        dataPoints: []
													    		});


 	  }
     // For External power delivery

 	   chartStatusBar.options.data.push({ 
 	   	                            click: redirect_to_detail,

										 	   	        indexLabel: "{y}",
										              indexLabelPlacement: "inside",  
										              indexLabelOrientation: "horizontal",
										              indexLabelFontColor: "#111111",
                                  indexLabelFontSize: 12,
													    		markerType:"none",	    
													    	  lineThickness:0, 
													    	  type: "stackedColumn",
													    	  color:"#00AA00",
													    	  
													        fillOpacity: .5, 
													        xValueType: "number",
													        axisYType:"secondary",
													        dataPoints: []
													    		});


 	   chartStatusBar.options.data.push({   
													    		markerType:"none",	    
													    	  lineThickness:0, 
													    	  type: "stackedColumn",
													    	  color:"#003300",
													        fillOpacity: .5, 
													        xValueType: "number",
													        axisYType:"secondary",
													        dataPoints: []
													    		});

 	   chartDayHourSum.options.data.push({   
													    		markerType:"none",	    
													    	  lineThickness:0, 
													    	  type: "stackedArea",
													        fillOpacity: .4, 
													        xValueType: "dateTime",
													        axisYType:"secondary",
													        dataPoints: []
													    		});
													    		
      chartDayHourSum.options.data.push({   
 	   															//indexLabel: "{y}",
							                    toolTipContent:"{y} W",
										              indexLabelPlacement: "inside",  
										              indexLabelOrientation: "horizontal",
										              indexLabelFontColor: "#111111",
                                  indexLabelFontSize: 12,
													    		markerType:"none",	    
													    	  color:"transparent", 
													    	  type: "line",
													        xValueType: "dateTime",
													        axisYType:"secondary",
													        dataPoints: []
													    		});													    		
 	 //dummys for left Border
 	 chartDayHourSum.options.data.push({ axisYType:"primary", dataPoints: []	});
 	 chartDayLogSum.options.data.push({ axisYType:"primary", dataPoints: []	});													    		
 	 chartStatusBar.options.data.push({ axisYType:"primary", dataPoints: []	});													    		

   initComboReduce();
 	 refresh_data();
 	 
 	 getDayHourChartData();  // step/hour diagramm
 	 getDaySumChartData();  // gesamtuebersicht 
 	 
 	 
 	 window.parent.fillCanvas(document.getElementById('Canvas2'),'#EEF0FF','#C1C6FF');
   window.parent.fillCanvas(document.getElementById('Canvas1'),'#EFF1FF','#EEF0FF');
 }
 
 function initComboReduce() {
  	if (devicedata == null) {
  		setTimeout('initComboReduce();',100); 
  	  return;
  		}
  	var elem = document.getElementById('maxpower');
  	elem.innerHTML = "von " + devicedata.sumpower+ " W";
  	elem = document.getElementById('combo_reduce');
  	for (a=0; a<=devicedata.sumpower; a+=10) {
  		var opt = document.createElement("option");
  		if (a-5 < devicedata.setsumpower && a+5 >= devicedata.setsumpower) 
  			opt.selected=true;
  		opt.value =a;
  		opt.text=a;
  		elem.appendChild(opt);
  	}
  }
</script>
</head>

<body onloadx="initCharts();">
<div style='width:100%%; height:100%%;background-color:transparent'>

<div style='display:none; position:absolute; margin:0px;top:8px; right:25px;font-size:14px;font-family:arial;z-index:60;'>	
	<div style='display:inline-block;'>Leistung max.</div>
	<div style=';display:inline-block;'>	
	     <select onchange='set_reduction();' id='combo_reduce' size='1' name='rcombo' style='padding:2px;border-radius: 5px;border:2px solid #8888aa;font-size:16px;background-color:#FAFAFF;'>
	     </select>	
	</div>
	<div id='maxpower'style='display:inline-block;'></div>
</div>

<div id="info" style='position:absolute; top:500px; left:0px; height: 50px; width:100px;z-index:100; border:1px;margin-left:0px;'></div>

<div style='position:absolute; top:50px; right:25px;height:170px;width:260px;border:0px solid #aaaaaa;z-index:60;'>
  <table onclick='reload_daten("table_statistik","STATISTIK");' id='table_statistik' style='vertical-align:text-top;	width:100%%;margin-top:0px;cursor:pointer;	'></table>
	</div>

|-------- Charts

<div style='position:absolute; top:17px; left:330px;width:220px; text-align:center; font-size:13px;font-family:arial;' >Ertrag PV / Zulieferung EV</div>	
<div style='position:absolute; top:17px; left:20px; width:230px; text-align:center;font-size:13px;font-family:arial;' >Ertrag alle PV heute / Stunde</div>

<div id="chartDayLogSumID" style='position:absolute; top:280px; left:0px; height: 275px; width:895px;z-index:111; border:0px solid red;margin:0px;'></div>
<div id="chartDayHourSumID" style='position:absolute; top:25px; left:0px; height: 270px; width:290px;z-index:100; border:0px solid black;margin-left:0px;'></div>
<div id="chartStatusBarID" style='position:absolute; top:	25px; left:300px; height: 270px; width:290px;z-index:100; border:0px solid black;margin:0px;'></div>



<canvas id="Canvas2" style='display:block;position:absolute; top:280px; left:0px; height: 260px; width:100%%; z-index:105; border-radius: 6px;'></canvas>
<canvas id="Canvas1" style='display:block;position:absolute; top:0px; left:0px; height: 100%%; width:100%%; z-index:-100;border-radius: 6px; '></canvas>
<script type="text/javascript">
	initCharts();
</script>	
</body></html>



| ******************************************************************* Log Analyse *******************************************************************

##80
<!DOCTYPE html>
<html><head>
	
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />

	
<script type="text/javascript">

  var numdevices=window.parent.maxdevices;
  var hosttxt=window.parent.hosttxt;
  var devicefield;
  var chartSelectBar;	
  var devicedata = window.parent.devicedata;
  var granvalue; 
  var granyear;
  var selectedDay;
  var Timeout_ArchiveLogChartData;
  
  function initDeviceCombo() {
  	if (devicedata == null) {
  		setTimeout('initDeviceCombo(0);',100); 
  	  return;
  		}
  	var elem = document.getElementById('combo_device');
  	for (a=-1; a<devicedata.devices.length; a++) {
  		var opt = document.createElement("option");
  		if (a==-1) {
  			opt.selected=true;
  		  opt.value =-1;
  		  opt.text='Alle Wechselrichter';
  		  opt.style.fontWeight='bold';
  		  }
  		else {
  			 opt.value =devicedata.devices[a].id;
  		   opt.text=devicedata.devices[a].name;
  	    }
  		elem.appendChild(opt);
  	}
  	createCharts();
  }
  
  function refreshSelectChart() 
   {
   	 var cbo = document.getElementById('combo_device');
   	 var elem = document.getElementById('combo_chart');
     if (cbo.options[cbo.selectedIndex].value == -1) { // alle WR ausgewählt in combo_charts nur wert > 100 zulassen
     	  var a;
     	  for (a=0; a<elem.options.length; a++) {
     	     if (elem.options[a].value < 100) {
     	        elem.options[a].disabled = true;
     	       } 
     	     else {
     	     	  elem.options[a].disabled = false;
     	     	  if (elem.options[a].value == 101)
     	     	    elem.options[a].selected = true;
     	     	  }
     	     } 
       }  	 
     else {  // einzel WR ausgewählt in combo_charts nur wert < 100 zulassen
     	  for (a=0; a<elem.options.length; a++) {
     	     if (elem.options[a].value > 100) {
     	        elem.options[a].disabled = true;
     	       } 
     	     else {
     	     	  elem.options[a].disabled = false;
     	     	  if (elem.options[a].value == 1)
     	     	    elem.options[a].selected = true;
     	     	  }
     	     } 
       }
     	  
   	
	   var rq;
	   rq = new XMLHttpRequest();
	   rq.onreadystatechange=function()
       {
       	if (rq.readyState==4 && rq.status==200)
         {
         	jdata = JSON.parse(rq.responseText);
         	parent.document.getElementById('id_current_ts').innerHTML=jdata.time;
         	chartSelectBar.options.data[numdevices].dataPoints=[];
         	if (getGranularity() == 1) {
		         	for (a=1; a<=window.parent.daysInMonth(granyear,granvalue-1); a++) {
		         		d = new Date(granyear, granvalue-1,a);
		         		var t = d.getTime();
		         		chartSelectBar.options.data[numdevices].dataPoints.push({x:t,y:0});
		         	}
           }
          else {
          			for (a=1; a<7; a++) {
		         		d = weeksToDate(granyear,granvalue,a);
		         		var t = d.getTime();
		         		chartSelectBar.options.data[numdevices].dataPoints.push({x:t,y:0});
		         	}
          }
          for (a=0; a<numdevices; a++) 
             chartSelectBar.options.data[a].dataPoints = [];
         	for (a=0; a<jdata.devices.length; a++) {
         		
             for (b=0; b<jdata.devices[a].data.length; b++) { 
             	   
            	       chartSelectBar.options.data[a].dataPoints.push({x:(jdata.devices[a].data[b].ts)*1000,y:jdata.devices[a].data[b].yield});
         	   }
          }
          chartSelectBar.render();
         }
       }
       var what = getGranularity() == 1 ? "MONTH" : "WEEK";
       var dev = document.getElementById('combo_device').options[document.getElementById('combo_device').selectedIndex].value;
       xrequest ="http://"+hosttxt+"/?CMD=GETLOG"+ what + "&DEV="+dev+"&YEAR="+granyear+"&VAL="+granvalue;
       rq.open("GET", xrequest, true);
	     rq.send(null);
   }
  
 function createCharts(){
 	   chartSelectBar = new window.parent.CanvasJS.Chart(document.getElementById("chartSelectBarID"),
					    {
	            zoomEnabled:true,
				    	animationEnabled: true,
				    	animationDuration: 200,
				    	backgroundColor:"transparent",
				    	margin: 0,

					    axisX:{ 
					    	     gridThickness: 1,
					        	 interval:1,
					        	 intervalType: "day",
					        	 valueFormatString :"DD",
					           labelFontColor: "#333333",
					           labelFontSize: 12,
					           margin:10,
					           lineThickness:1,
					           tickThickness:1
					    			},
		    			axisY: {
				    	       margin:5,
					           lineThickness:1,
					           tickThickness:0
					            },
					    axisY2: {
					    	     minimum:0,
					        	 gridThickness: 1,
				    	       labelFontColor: "#333333",
				    	       labelFontSize: 12,
				    	       margin:0,
					           lineThickness:1,
					           tickThickness:1
					            },
					    data: [
					    			]
					  	}
  	      );

 	chartDisplayLog = new window.parent.CanvasJS.Chart(document.getElementById("chartDisplayLogID"),
					    {
	            zoomEnabled:true,
				    	animationEnabled: false,
				    	animationDuration: 200,
				    	backgroundColor: "transparent",
				    	colorSet:"StackedSum",

					    axisX:{ 
					    	     gridThickness: 1,
					        	 interval:1,
					        	 intervalType: "hour",
					        	 valueFormatString :"HH",
					           labelFontColor: "#333333",
					           labelFontSize: 12,
					           margin:0,
					           lineThickness:1,
					           tickThickness:1
					    			},
		    			axisY: {
				    	       margin:5,
					           lineThickness:1,
					           tickThickness:0
					            },
					    axisY2: {
					    	     minimum:0,
					        	 gridThickness: 1,
				    	       labelFontColor: "#333333",
				    	       labelFontSize: 12,
				    	       margin:0,
					           lineThickness:1,
					           tickThickness:1
					            },
					    data: [
					    			]
					  	}
  	      );
  }

function initCharts(){
 	 createCharts();
 	 chartSelectBar.options.data=[];
 	 for (a=0; a<=numdevices; a++) {
 	   chartSelectBar.options.data.push({  
													    		click : loadLog, 
													    		markerType:"none",	    
													    	  lineThickness:0, 
													    	  type: "stackedColumn",
													        fillOpacity: .6, 
													        xValueType: "dateTime",
													        axisYType:"secondary",
													        dataPoints: []
													    		});
    }
    chartSelectBar.options.data.push({axisYType:"primary", dataPoints: []});
 } 
   
 function init_page(){
 	document.cookie="menu=wr-charting;";
 	initDeviceCombo();
 	setInitialDateRange();
 	initCharts();
 	window.parent.fillCanvas(document.getElementById('Canvas2'),'#EEF0FF','#C1C6FF');
  window.parent.fillCanvas(document.getElementById('Canvas1'),'#EFF1FF','#EEF0FF');
 }

 function weeksToDate(y,w,d) {
    var days = 2 + d + (w - 1) * 7 - (new Date(y,0,1)).getDay();
    return new Date(y, 0, days);
};
  
 Date.prototype.getWeek = function (dowOffset) {
/*getWeek() was developed by Nick Baicoianu at MeanFreePath: http://www.epoch-calendar.com */

	dowOffset = typeof(dowOffset) == 'int' ? dowOffset : 0; //default dowOffset to zero
	var newYear = new Date(this.getFullYear(),0,1);
	var day = newYear.getDay() - dowOffset; //the day of week the year begins on
	day = (day >= 0 ? day : day + 7);
	var daynum = Math.floor((this.getTime() - newYear.getTime() - 
	(this.getTimezoneOffset()-newYear.getTimezoneOffset())*60000)/86400000) + 1;
	var weeknum;
	//if the year starts before the middle of a week
	if(day < 4) {
		weeknum = Math.floor((daynum+day-1)/7) + 1;
		if(weeknum > 52) {
			nYear = new Date(this.getFullYear() + 1,0,1);
			nday = nYear.getDay() - dowOffset;
			nday = nday >= 0 ? nday : nday + 7;
			/*if the next year starts before the middle of
 			  the week, it is week #1 of that year*/
			weeknum = nday < 4 ? 1 : 53;
		}
	}
	else {
		weeknum = Math.floor((daynum+day-1)/7);
	}
	return weeknum;
};	 
  
 function getGranularity (){
 	 Combo=document.getElementById('combo_granularity');
   return Combo.options[Combo.selectedIndex].value;
  } 
  
 function showDateRange (){
 	 var gran =  getGranularity ();
   if (gran == 1) {
   	  document.getElementById('id_selecteddate').innerHTML=window.parent.month[granvalue] + " - " + granyear;
   	 }
   if (gran == 2) {
   	 document.getElementById('id_selecteddate').innerHTML="KW "+granvalue + " - " + granyear;
     }
   refreshSelectChart();  
  } 
  
 function setInitialDateRange() {
 	 var d = new Date();
   var gran =  getGranularity ();
   if (gran == 1) 
   	 granvalue = d.getMonth()+1;

   if (gran == 2) 
   	 granvalue = d.getWeek(1)+1;
   	 
   granyear = d.getYear()+1900 
   showDateRange ();
  } 
  
 function changeGranValue (val) {
 	  var gran =  getGranularity();
 	  granvalue+=val;
 	  
 	  if (gran == 1) { //Monat
		 	  if (granvalue == 0) {
		 	    granyear--;
		 	    granvalue = 12;
		 	  }
		 	   if (granvalue == 13) {
		 	    granyear++;
		 	    granvalue = 1;
		 	  }
      } 
    else {
    	 if (granvalue == 0) {
		 	    granyear--;
		 	    var d = new Date(granyear,11,31);
		 	    granvalue = d.getWeek();
		 	  }
    	 if (granvalue == 54) {
		 	    granyear++;
		 	    granvalue = 1;
		 	  }
    }  
     showDateRange(); 	
  } 
 
  var selectedSeries;
  
    function loadLog (e) {
  	if (e)
	    selectedSeries = e;
	  id = selectedSeries.dataSeriesIndex;
	  pt = selectedSeries.dataPointIndex;
	  d = new Date(selectedSeries.dataPoint.x);
	  selectedDay = d.getTime()/1000;
	  document.getElementById ( 'selectedDayID' ).innerHTML = d.getDate()+"."+window.parent.month[d.getMonth()+1]+" "+d.getFullYear();
	  document.getElementById ( 'selectedDayID' ).style.display='block';
 	  devicefield = "";
    var Combo = document.getElementById ( 'combo_device' );
    var deviceindex = Combo.options[Combo.selectedIndex].value;
    document.getElementById ( 'combo_chart_div' ).style.display='block';
    for (a=0; a<numdevices; a++)
     if (a==deviceindex || deviceindex == -1 )
       devicefield+="1";
     else  
       devicefield+="0";
	  document.getElementById ( 'selectedDayID' ).href= "http://"+hosttxt+"/?CMD=GETEXPORTFILE&DEV="+devicefield+"&TS="+selectedDay;
    document.getElementById ( 'selectedDayID' ).download="Exportfile.txt";
    document.getElementById ( 'selectedDayID' ).target="";

     var ndev;
     if (deviceindex == -1) 
        ndev = numdevices;
     else 
     	  ndev = 1;   
     
     chartDisplayLog.options.data = [];
     
     if (ndev == 1) {
			     chartDisplayLog.options.data.push({  
									    	  markerType:"none",
									    	  lineThickness:1, 
									    	  //color:"#00aa33",
									        type: "area",
									        fillOpacity: 0.6, 
									        xValueType: "dateTime",
									        axisYType:"secondary",
									        dataPoints: []
									    		});
			     chartDisplayLog.options.data.push({  
									    	  markerType:"none",
									    	  lineThickness:1, 
									    	  //color:"#aa3300",
									        type: "line",
									        fillOpacity: 0.6, 
									        xValueType: "dateTime",
									        axisYType:"primary",
									        dataPoints: []
									    		});
     }

     if (ndev > 1) {
     	   // data[0] = summe über alle wr
		     chartDisplayLog.options.data.push({  
								    	  markerType:"none",
								    	  lineThickness:0.5, 
								    	  //color:"#00aa33",
								        type: "line",
								        fillOpacity: 0.6, 
								        xValueType: "dateTime",
								        axisYType:"secondary",
								        dataPoints: []
								    		});

		     for (a=0; a<ndev; a++) { // Einzelwerte als Liniendiagramm
				     chartDisplayLog.options.data.push({  
										    	  markerType:"none",
										    	  lineThickness:0.4, 
										        type: "line",
										        color:"#ff0000",
										        fillOpacity: 0.5, 
										        xValueType: "dateTime",
										        axisYType:"secondary",
										        dataPoints: []
										    		});
		     }
     }
     
    chartDisplayLog.options.data.push({ axisYType:"primary", dataPoints: []});
    
    document.getElementById ( 'chartDisplayLogDummy' ).style.display="none";
       
    if (ndev == 1) {
    		 var Combo = document.getElementById ( 'combo_chart' );
			   var ctyp = parseInt(Combo.options[Combo.selectedIndex].value);
			   clearvaluefield();
			   switch (ctyp) {
			   	  case 1  : setvaluefield(3,1); // power ac
		                  setvaluefield(0,1); // reduce
		                  chartDisplayLog.options.axisY.labelFontColor = "#333333";
		                  chartDisplayLog.options.axisY2.labelFontColor = "#333333";
			     		        chartDisplayLog.options.data[1].axisYType = "secondary";
			     		        chartDisplayLog.options.data[1].type = "line";
		                  chartDisplayLog.options.data[1].color = "#ee0000";
			   	            break;
			   	  case 2 : setvaluefield(2,1); // power dc
		                  setvaluefield(0,1); // reduce
		                  chartDisplayLog.options.axisY.labelFontColor = "#333333";
		                  chartDisplayLog.options.axisY2.labelFontColor = "#333333";
			     		        chartDisplayLog.options.data[1].axisYType = "secondary";
			     		        chartDisplayLog.options.data[1].type = "line";
		
			   	            break;
			   	  case 3  : setvaluefield(2,1); // power dc
		                  setvaluefield(4,1); // voltage dc
		                  chartDisplayLog.options.axisY.labelFontColor = chartDisplayLog.options.data[1].color;
		                  chartDisplayLog.options.axisY2.labelFontColor = chartDisplayLog.options.data[0].color;
			     		        chartDisplayLog.options.data[1].axisYType = "primary";
			     		        chartDisplayLog.options.data[1].type = "line";
			   	            break;
			   	  case 4  : setvaluefield(2,1); // power dc
		                  setvaluefield(5,1); // current dc
		                  chartDisplayLog.options.axisY.labelFontColor = chartDisplayLog.options.data[1].color;
		                  chartDisplayLog.options.axisY2.labelFontColor = chartDisplayLog.options.data[0].color;
		       		        chartDisplayLog.options.data[1].axisYType = "primary";
		       		        chartDisplayLog.options.data[1].type = "line";
			   	            break;
			   	  case 5   : setvaluefield(3,1); // power ac
		                  setvaluefield(7,1); // current ac
		                  chartDisplayLog.options.axisY.labelFontColor = chartDisplayLog.options.data[1].color;
		                  chartDisplayLog.options.axisY2.labelFontColor = chartDisplayLog.options.data[0].color;
		       		        chartDisplayLog.options.data[1].axisYType = "primary";
		       		        chartDisplayLog.options.data[1].type = "line";
			   	            break;
			   	  case 6   : setvaluefield(3,1); // power ac
		                  setvaluefield(6,1); // voltage dc
		                  chartDisplayLog.options.axisY.labelFontColor = chartDisplayLog.options.data[1].color;
		                  chartDisplayLog.options.axisY2.labelFontColor = chartDisplayLog.options.data[0].color;
		       		        chartDisplayLog.options.data[1].axisYType = "primary";
		       		        chartDisplayLog.options.data[1].type = "line";
			   	            break;
			     }
		    	getArchiveLogChartData(1,ctyp,1);   
		 }
		 
		 if (ndev > 1) {
    		 var Combo = document.getElementById ( 'combo_chart' );
			   var ctyp = parseInt(Combo.options[Combo.selectedIndex].value);
			   clearvaluefield();
			   switch (ctyp) {
			   	  case 101 : setvaluefield(3,1); // power ac
		                  chartDisplayLog.options.axisY.labelFontColor = "#333333";
		                  chartDisplayLog.options.axisY2.labelFontColor = "#333333";
			   	            break;
			   	  case 102 : setvaluefield(2,2); // power dc
		                  setvaluefield(0,1); // reduce
		                  chartDisplayLog.options.axisY.labelFontColor = "#333333";
		                  chartDisplayLog.options.axisY2.labelFontColor = "#333333";
		                  break; 
			     }
		     getArchiveLogChartData(1,ctyp);   
		 } 
  }
  
		var valuefield="0000000000";
		
		function clearvaluefield() {
			valuefield="0000000000";
		}
		
		function setvaluefield(pos, val) {
			valuefield = valuefield.split('');
		  valuefield[pos] = String.fromCharCode(48+val);
			valuefield = valuefield.join('');
		}

  
   function getArchiveLogChartData( clearts, ctyp ) 
   {
	   var rq;
	   rq = new XMLHttpRequest();
	   rq.onreadystatechange=function() {
       	if (rq.readyState==4 && rq.status==200)
         {
        	jdata = JSON.parse(rq.responseText);
          parent.document.getElementById('id_current_ts').innerHTML=jdata.time;
           
           var ce = chartDisplayLog.options.data;
				   if (clearts) {
				   	 for (c=0; c< chartDisplayLog.options.data.length; c++) {
				        chartDisplayLog.options.data[c].dataPoints = [];
				      }
				    }  
          
           if (ctyp < 100)  // nur ein WR         	 
             for (b=0; b<jdata.devices[0].data.length; b++) { 
             	 var je = jdata.devices[0].data[b];
						   switch (ctyp) {
						   	  case 1:   ce[0].dataPoints.push({x:je.ts*1000,y:je.pac});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.red});
						   	          break;
						   	  case 2:   ce[0].dataPoints.push({x:je.ts*1000,y:je.pdc});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.red});
						   	          break;
						   	  case 3:   ce[0].dataPoints.push({x:je.ts*1000,y:je.pdc});
  														ce[1].dataPoints.push({x:je.ts*1000,y:je.vdc});
						   	          break;						   	          
						   	  case 4:   ce[0].dataPoints.push({x:je.ts*1000,y:je.pdc});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.cdc});
						   	          break;						   	          
						   	  case 5:   ce[0].dataPoints.push({x:je.ts*1000,y:je.pac});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.cac});
						   	          break;						   	          
						   	  case 6:   ce[0].dataPoints.push({x:je.ts*1000,y:je.pac});
			                	      ce[1].dataPoints.push({x:je.ts*1000,y:je.vac}); // effektivstrom
						   	          break;						   	          
						    }
        	  }
 
           if (ctyp > 100 ) // SUMME und einzelwerte
            for (a=0; a<jdata.devices.length; a++) { 
             for (b=0; b<jdata.devices[a].data.length; b++) { 
             	 var je = jdata.devices[a].data[b];
						   switch (ctyp) {
						   	  case 101: ce[a].dataPoints.push({x:je.ts*1000,y:je.pac});
						   	          break;
						   	  case 102: ce[a].dataPoints.push({x:je.ts*1000,y:je.pdc});
						   	          break;

						    }
        	  }       	
        	}
          chartDisplayLog.render();
         }
       }
	   if (clearts) {
	      lasttsDayLog = 0;
        document.getElementById("combo_chart").blur();
	      }
       xrequest ="http://"+hosttxt+"/?CMD=GETARCHIVELOGDATA&DEV="+devicefield+"&TS="+selectedDay+"&VAL="+valuefield;
       rq.open("GET", xrequest, true);
       rq.send(null);
   }
  
</script></head>

<body onloadx="init_page();" >
<div>
 <div style='position:absolute; margin:0px;top:5px;right:5px;font-size:15px;width:290px;font-family:arial; z-index:150;border: 0px solid blue;'>	
    <select onchange='setInitialDateRange();' id='combo_granularity'  size='1' name='rcombo' style='padding:2px;border-radius: 5px;border:2px solid #8888aa;margin:5px;font-size:16px;background-color:#FAFAFF;'>
	     	 <option value='1' selected>Monat</option>
	     	 <option value='2' >Woche</option>
	     </select>
	  <input onclick='changeGranValue(-1);' style='position:absolute; margin:0px;top:3px; left:92px;text-align:center;cursor:pointer;font-family: Arial;  font-size: 25px; width:30px; padding-top:0px; height:30px;border-radius: 3px;line-height:5px;' type='submit' value='-' >	 
    <div style='position:absolute; margin:0px;top:10px; right:35px;width:130px; font-size:15px;font-weight:bold;text-align:center;border:0px solid red;'id='id_selecteddate'>KW 15 2015</div>
    <input onclick='changeGranValue(1);' style='position:absolute; margin:3px;top:0px;right:0px;text-align:center;cursor:pointer;font-family: Arial;  font-size: 25px; width:30px; height:30px;border-radius: 3px;line-height:1px;' type='submit' value='+' >	     
   
    <div style='position:absolute; margin:0px;top:56px; left:12px;width:150px; font-size:16px;font-weight:normal;text-align:left;border:0px solid red;'>Auswahl:</div>		
    <select  onchange='refreshSelectChart();' id='combo_device' size='1' style='position:absolute; top:	50px; right:5px;width:200px;padding:2px;border-radius: 5px;border:2px solid #8888aa;font-size:16px;'>
	  </select>	
   <div style='position:absolute; margin:0px; top:86px; left:12px;width:150px; font-size:16px;font-weight:normal;text-align:left;border:0px solid red;'></div>
</div>

<div id='combo_chart_div' style='display:none;position:absolute; margin:0px;top:210px; right:10px;font-size:15px;font-family:arial; z-index:150;'>	
		<form style='cursor:default; margin:0px; padding:0px;'  target=''    >
	     <select onchange='loadLog() ; ' id='combo_chart' size='1' style='width:280px;padding:2px;border-radius: 5px;border:2px solid #8888aa;font-size:16px;background-color:#FAFAFF;'>
	     	   <option value='1' selected>AC: Leistung & Begrenzung</option>
	     	   <option value='2'         >DC: Leistung & Begrenzung</option>
	     	   <option value='3'         >DC: Leistung & Spannung</option>
	     	   <option value='4'         >DC: Leistung & Strom</option>
	     	   <option value='5'         >AC: Leistung & Strom</option>
	     	   <option value='6'         >AC: Leistung & Spannung</option>
	     	   <option value='101'        >AC: Leistung kumuliert</option>
	     	   <option value='102'        >DC: Leistung kumuliert</option>
	     </select>	
	  </form>
	
</div>

<a id='selectedDayID' style='color: #000000;text-decoration:none;display:none;position:absolute; margin:0px;top:235px; text-align:center;left:400px;width:130px;font-size:16px;padding:3px;font-weight:normal;font-family:arial; background-color:#ffffff; z-index:150;border:2px solid #aaaaaa; border-radius:5px;background-color:#FAFAFF;'></a>

<div id="chartSelectBarID" style='position:absolute; top:	5px; left:10px; height: 250px; width:550px;z-index:100; border:0px solid black;margin:0px;'></div>
<div id="chartDisplayLogDummy" style='position:absolute; top:	242px; left:10px; font-family:arial; font-size:30px;line-height:200px; text-align:center;height: 315px; width:880px;z-index:111; background-color:transparent;border:0px solid black;margin:0px;'>Log-Tag zur Anzeige auswählen</div>
<div id="chartDisplayLogID" style='position:absolute; top:	242px; left:10px; height: 315px; width:880px;z-index:111;margin:0px;'></div>

<canvas id="Canvas2" style='display:block;position:absolute; top:240px; left:0px; height: 300px; width:100%%; z-index:105; border-radius: 6px;'></canvas>
<canvas id="Canvas1" style='display:block;position:absolute; top:0px; left:0px; height: 100%%; width:100%%; z-index:-100;border-radius: 6px; '></canvas>

</div>
<script type="text/javascript">
	init_page();
</script>	
</body></html>

|************************************************************** OFFLINE **************************************************************************
##100
<!DOCTYPE html>
<html><head>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />
<script type="text/javascript">
function resizeaction() {
	var l;
  var elem = document.getElementById('id_div_offline').style;
  l = window.innerWidth / 2 - 200;
	elem.left = l+"px";
}
</script
</head>

<body onload=" resizeaction();">
<div style='width:100%%; height:100%%;background-color:transparent; margin:auto'>

<div id='id_div_offline' style='position:absolute;margin:auto;top:200px;width:400px; border:0px solid blue;text-align:center;font-family:arial; line-height:40px;;font-size:30px;color:#cc0000;' >%s Wechselrichter OFFLINE</div>

<div>

</body>
</html>


#
