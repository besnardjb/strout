function hasSpace(s)
{
	if( s == undefined )
		return -1;


	if( s.indexOf(" ") != -1 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}	



var Action = function( line )
{
	/* Members */
	this.args = {};
	this.path = [];
	this.time = 0.0;

	/* Constructror */
	var content = /([0-9\.\,eE-\\+]+):(.*): (.*)$/.exec(line);

	if( typeof(content) != typeof([]) )
	{
		throw new Error("Factory pattern does not match");
	}

	/* Time */
	this.time = content[1];
	
	/* Path */
	var p = content[2];

	if( hasSpace(p) != 0 )
	{
		throw new Error(line + " : Contains space in path");
		return;
	}

	this.path = p.split(":");

	/* Args */
	var a = content[3];

	var rge = /([^\s\"]+):((?:[^\s\"]+)|"(?:.*)")?/g;
	sa = rge.exec(a);

	while(sa!=null)
	{
		var k = sa[1];
		var v = sa[2];

		if(hasSpace(k) != 0)
		{
			throw new Error(line + "Found argument key with space")
			return;
		}

		if( v == undefined )
		{
			v="";
		}

		this.args[k] = v.replace(/(^"|"$)/g, "");
		
		sa = rge.exec(a);
	}



	//console.log(this);

	/* Methods */
	Action.prototype.gettype = function()
	{
		return this.type;
	}
	
	Action.prototype.getargs = function()
	{
		return this.args;
	}

	Action.prototype.getpath = function()
	{
		return this.path;
	}

}

module.exports.Action = Action;
