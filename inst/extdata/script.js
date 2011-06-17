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
			nc.id=picId;
			var newpic=document.createElement('img');
			newpic.src=this.href;
			if (self.pageYOffset)
			{
				nc.style.top = self.pageYOffset + "px";
			}
			else if (document.documentElement && document.documentElement.scrollTop)
			{
				nc.style.top = document.documentElement.scrollTop + "px";
			}
			else if (document.body.scrollTop)
			{
				nc.style.top = document.body.scrollTop + "px";
			}
			else
			{
				nc.style.top = 0 + "px";
			}
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