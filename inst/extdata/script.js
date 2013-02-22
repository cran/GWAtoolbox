window.onload = function()
{
	if(document.getElementById && document.createTextNode)
	{
		assign();  
	}
}

function assign()
{
	var picId='bigfigure';
	var piclinks=document.getElementsByTagName('a');
	for(var i=0;i<piclinks.length;i++)
	{
		piclinks[i].onclick=function()
		{
			var oldp=document.getElementById(picId);
			if(oldp)
			{
				oldp.parentNode.removeChild(oldp);
			}
			var nc=document.createElement('div');
			this.parentNode.parentNode.insertBefore(nc, this.parentNode);
			nc.style.display='none';
			nc.style.zIndex = "1";
			nc.id=picId;
			var newpic=document.createElement('img');
			newpic.src=this.href;
			nc.style.top = 0 + "px";
			newpic.alt=this.getElementsByTagName('img')[0].alt;
			newpic.title='Click to close the figure';
			newpic.onclick=function()
			{
				this.parentNode.parentNode.removeChild(this.parentNode);
			}
			nc.appendChild(newpic);
			nc.style.display='block';
			return false;
		}
	}    
}

function minmax(element) {
	if (element.innerHTML == "+") {
		element.innerHTML = "-";
		div = element.nextSibling;	
		while (div != null) {
			if (div.nodeType == 1) {
				div.style.display = "block";
			}
			div = div.nextSibling;
		}
	} else if (element.innerHTML == "-") {
		element.innerHTML = "+";	
		div = element.nextSibling;	
		while (div != null) {
			if (div.nodeType == 1) {
				div.style.display = "none";
			}
			div = div.nextSibling;
		}
	}		
}