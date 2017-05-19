var act = require("./action");
var crypto = require("crypto");
var fs = require("fs");


var Page = function( manager, path, title )
{
	/* Members */

	this.path = "";
	this.content = "";
	this.title = "";
	this.mymanager = null;

	/* Methods */
	Page.prototype.write = function()
	{
		var data = this.mymanager.header(this.title);
		data += this.content;
		data += this.mymanager.footer();


		fs.writeFileSync( this.path, data );
	}

	Page.prototype.append = function( data )
	{
		this.content += data;
	}

	Page.prototype.clear = function( data )
	{
		this.content = "";
	}


	/* Constructor */
	this.path = path;
	this.title = title;
	this.mymanager = manager;
}

var PageManager = function(path)
{
	/* Members */
	this.pages = {};
	this.path = process.cwd() + "/report/"
	this.resid = 0;

	/* Methods */
	PageManager.prototype.get = function( id )
	{
		return this.pages[id];
	}


	PageManager.prototype.header = function(title)
	{
		return  "<html>\n<head>\n<title>"+ title +"</title>\n<body>\n";
	}

	PageManager.prototype.footer = function(title)
	{
		return  "</body>\n</html>\n";
	}

	PageManager.prototype.create = function( id, title )
	{
		if( this.get(id) )
		{
			throw new Error( id + " Page already exists");
			return;
		}

 		var shasum = new crypto.createHash('sha1');
		shasum.update( id );
		
		this.pages[id] = new Page( this, this.path + "/" + shasum.digest('hex') + ".html", title );
		return this.pages[id];
	}

	PageManager.prototype.getrespath = function()
	{
		this.resid++;
		return this.path + "/res/" + this.resid;
	}


	/* Constructor */
	if( path )
	{
		this.path = path;
	}

    fs.mkdirSync( this.path );
    fs.mkdirSync( this.path + "/res/" );

}



module.exports.PageManager = new PageManager();



