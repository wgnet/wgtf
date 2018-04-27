pragma Singleton
import QtQuick 2.5
import WGControls 2.0

/* Creates JIRA bugs!

Example Bug in Global Jira:
\code{.js}
    WGJira.jiraLink("Sample Bug Description", "Sample Bug Title", "Sample Callstack","https://jira.wargaming.net","1","29480")
\endcode

Example Bug in WGS Jira:
\code{.js}
    WGJira.jiraLink("Sample Bug Description", "Sample Bug Title", "Sample Callstack","https://jira-wgs.wargaming.net","1","10503")
\endcode

NOTE: There is a character limit on the URL created.

*/

Item {
    WGComponent { type: "WGJira" }
    id: jiraFunctions

    function jiraLink(description, summary, quote, server, issueType, pid)
    {
        if (typeof server == 'undefined' || server == "")
        {
            server = "http://jira.wargaming.net/secure/CreateIssueDetails!init.jspa?"
        }
        else
        {
            server = server + "/secure/CreateIssueDetails!init.jspa?"
        }

        if (typeof pid == 'undefined' || pid == "")
        {
            pid = "pid=29480"
        }
        else
        {
            pid = "pid=" + pid
        }

        if (typeof issueType == 'undefined' || issueType == "")
        {
            issueType = "&issuetype=1"
        }
        else
        {
            issueType = "&issuetype=" + issueType
        }

        if (typeof description != 'undefined' && description != "")
        {
            description = "&description=" + description
        }

        if (typeof quote != 'undefined' && quote != "")
        {
            quote = "+%7Bquote%7D+" + quote + "+%7Bquote%7D+"
            description += quote
        }

        if (typeof summary != 'undefined' && summary != "")
        {
            summary = "&summary=" + summary
        }
        else
        {
            summary = "&summary=WTF+Bug"
        }

        var jiraURL = server
                + pid
                + issueType
                + summary
                + description

        jiraURL = jiraURL.replace(/ /g, '+');

        Qt.openUrlExternally(jiraURL)
    }
}
