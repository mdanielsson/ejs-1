/*
    Test parent(), elements(), attributes(), attribute()
 */

var xml: XML =
<order>
   <customer>
        <name>Joe Green</name>
        <address>
            <street status="verified">410 Main</street>
               <city>Seattle</city>
                <state tax="taxable">WA</state>
       </address>
    </customer>
   <customer>
        <name>Peter Smith</name>
        <address>
            <street>777 Park Ave</street>
               <city>York</city>
            <state>VT</state>
       </address>
    </customer>
</order>


assert(xml.customer.address.name() == "address")
assert(xml.customer.address.parent().name() == "customer")
assert(xml.elements().length() == 2)
assert(xml.customer[0].address.street.attributes() == "verified")
assert(xml.customer[0].address.street.attribute("status") == "verified")
assert(xml.parent() == null)
