/*
  	Test descender get access
 */

var order : XML = <order>
	<customer><name>Joe Green</name>
		<address><street>410 Main</street>
			<city>New York</city><state>VT</state>
		</address>
	 </customer>
	<item level="rush" priority="low">
		<price>2.50</price>
		<qty>30</qty>
	</item>
	<item level="normal">
		<price>1.50</price>
		<qty>10</qty>
	</item>
</order>

assert(order..qty.length() == 2)
assert(order..city == "New York")
assert(order..@* == "rush low normal")
