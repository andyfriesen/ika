
    function quoteText(id)
    {
        var name = self.document.getElementById("name" + id).textContent;
        var text = self.document.getElementById("text" + id).innerHTML;
        text = text.replace(/<.+>/g, "");
        self.document.forms[0].elements[2].value += '<quote name="' + name + '">' + text + '</quote>';
    }
