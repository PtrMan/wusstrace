<!-- 
    Copyright notice
    ================

    Copyright (C) 2010
	Lorenzo Martignoni <martignlo@gmail.com>
	Roberto Paleari <roberto.paleari@gmail.com>

    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 51
    Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
-->


<!-- 
     to use, just add the following line as second line inside the XML syscall
     dump file

     <?xml-stylesheet type="text/xsl" href="wusstrace.xsl" ?> 
-->

<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method='html' version='1.0' encoding='UTF-8' indent='yes'/>

  <xsl:template match="/">
    <html>
      <body>
	<h2>Syscall Trace</h2>
	<table border="1">
	  <tr bgcolor="#9acd32">
            <th align="left">Number</th>
            <th align="left">Name</th>
            <th align="left">Direction</th>
            <th align="left">Thread</th>
            <th align="left">Arguments</th>
	  </tr>
	  <xsl:for-each select="boost_serialization/Syscall">
	    <tr>
              <td><xsl:value-of select="SyscallNo"/></td>
              <td><xsl:value-of select="SyscallName"/></td>
              <td><xsl:value-of select="SyscallDir"/></td>
              <td><xsl:value-of select="ThreadID"/></td>
	      <td>
		<table>
		  <xsl:for-each select="SyscallArg">
		    <tr>
		      <td><xsl:value-of select="ArgNumber"/></td>
		      <xsl:choose>
			<xsl:when test="ArgDirection = 0">
			  <td bgcolor="#c3c3c3">NONE</td>
			</xsl:when>
			<xsl:when test="ArgDirection = 1">
			  <td bgcolor="#28e33a">IN</td>
			</xsl:when>
			<xsl:when test="ArgDirection = 2">
			  <td bgcolor="#2880e3">OUT</td>
			</xsl:when>
			<xsl:when test="ArgDirection = 3">
			  <td bgcolor="#ffc871">INOUT</td>
			</xsl:when>
			<xsl:otherwise>
			  <td bgcolor="#ff7192">unknown direction</td>
			</xsl:otherwise>
		      </xsl:choose>
		      <xsl:choose>
			<xsl:when test="ArgOptional = 0">
			  <td>NONE</td>
			</xsl:when>
			<xsl:when test="ArgOptional = 1">
			  <td>OPTIONAL</td>
			</xsl:when>
			<xsl:when test="ArgOptional = 2">
			  <td>MANDATORY</td>
			</xsl:when>
			<xsl:otherwise>
			  <td>unknown optional/mandatory</td>
			</xsl:otherwise>
		      </xsl:choose>
		    </tr>
		  </xsl:for-each>
		</table>
	      </td>
	    </tr>
	  </xsl:for-each>
	</table>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
