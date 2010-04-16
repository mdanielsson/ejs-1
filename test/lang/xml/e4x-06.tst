/*
    Parse document with CDATA
 */

let xmlstring = "
    <?xml version=\"1.0\" encoding=\"UTF-8\" ?>
    <response uri=\"/api/private/xml/documents\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"www.acme.com/doc.xsd\">
        <!-- comment -->
        <result>
            <documents>
                <document>
                    <documentId><![CDATA[5452001234067001]]></documentId>
                    <documentName><![CDATA[abc]]></documentName>
                    <version><![CDATA[1.0]]></version>
                    <lastModifiedTime><![CDATA[17588880634]]></lastModifiedTime>
                    <lastModifiedBy><![CDATA[zembosa]]></lastModifiedBy>
                    <writePermission><![CDATA[false]]></writePermission>
                    <document_name_url><![CDATA[abc]]></document_name_url>
                    <shared_users><![CDATA[[]]]></shared_users>
                    <document_locked><![CDATA[false]]></document_locked>
                    <document_blogged><![CDATA[false]]></document_blogged>
                    <authorName><![CDATA[zembosa]]></authorName>
                    <created_date><![CDATA[754659990528]]></created_date>
                    <category>owned</category>
                </document>
            </documents>
        </result>
    </response>";

let xml = new XML(xmlstring);
