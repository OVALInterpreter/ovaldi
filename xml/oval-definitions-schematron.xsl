<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<xsl:stylesheet xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                xmlns:sch="http://www.ascc.net/xml/schematron"
                xmlns:iso="http://purl.oclc.org/dsdl/schematron"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:oval="http://oval.mitre.org/XMLSchema/oval-common-5"
                xmlns:oval-def="http://oval.mitre.org/XMLSchema/oval-definitions-5"
                xmlns:ind-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#independent"
                xmlns:aix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#aix"
                xmlns:apache-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#apache"
                xmlns:catos-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#catos"
                xmlns:esx-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#esx"
                xmlns:freebsd-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#freebsd"
                xmlns:hpux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#hpux"
                xmlns:ios-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#ios"
                xmlns:linux-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#linux"
                xmlns:macos-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#macos"
                xmlns:sol-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#solaris"
                xmlns:unix-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix"
                xmlns:win-def="http://oval.mitre.org/XMLSchema/oval-definitions-5#windows"
                version="2.0"
                xsi:dummy-for-xmlns=""
                oval:dummy-for-xmlns=""
                oval-def:dummy-for-xmlns=""
                ind-def:dummy-for-xmlns=""
                aix-def:dummy-for-xmlns=""
                apache-def:dummy-for-xmlns=""
                catos-def:dummy-for-xmlns=""
                esx-def:dummy-for-xmlns=""
                freebsd-def:dummy-for-xmlns=""
                hpux-def:dummy-for-xmlns=""
                ios-def:dummy-for-xmlns=""
                linux-def:dummy-for-xmlns=""
                macos-def:dummy-for-xmlns=""
                sol-def:dummy-for-xmlns=""
                unix-def:dummy-for-xmlns=""
                win-def:dummy-for-xmlns="">

<!--PHASES-->


<!--PROLOG-->


<!--KEYS-->


<!--DEFAULT RULES-->


<!--MODE: SCHEMATRON-FULL-PATH-->
<xsl:template match="*|@*" mode="schematron-get-full-path">
      <xsl:apply-templates select="parent::*" mode="schematron-get-full-path"/>
      <xsl:text>/</xsl:text>
      <xsl:choose>
         <xsl:when test="namespace-uri()=''">
            <xsl:value-of select="name()"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:text>*:</xsl:text>
            <xsl:value-of select="local-name()"/>
            <xsl:text>[namespace-uri()='</xsl:text>
            <xsl:value-of select="namespace-uri()"/>
            <xsl:text>']</xsl:text>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:variable name="preceding"
                    select="count(preceding-sibling::*[local-name()=local-name(current())                                   and namespace-uri() = namespace-uri(current())])"/>
      <xsl:text>[</xsl:text>
      <xsl:value-of select="1+ $preceding"/>
      <xsl:text>]</xsl:text>
   </xsl:template>
   <xsl:template match="@*" mode="schematron-get-full-path">
      <xsl:apply-templates select="parent::*" mode="schematron-get-full-path"/>@*[local-name()='schema' and namespace-uri()='http://purl.oclc.org/dsdl/schematron']</xsl:template>

   <!--MODE: GENERATE-ID-FROM-PATH -->
<xsl:template match="/" mode="generate-id-from-path"/>
   <xsl:template match="text()" mode="generate-id-from-path">
      <xsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
      <xsl:value-of select="concat('.text-', 1+count(preceding-sibling::text()), '-')"/>
   </xsl:template>
   <xsl:template match="comment()" mode="generate-id-from-path">
      <xsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
      <xsl:value-of select="concat('.comment-', 1+count(preceding-sibling::comment()), '-')"/>
   </xsl:template>
   <xsl:template match="processing-instruction()" mode="generate-id-from-path">
      <xsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
      <xsl:value-of select="concat('.processing-instruction-', 1+count(preceding-sibling::processing-instruction()), '-')"/>
   </xsl:template>
   <xsl:template match="@*" mode="generate-id-from-path">
      <xsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
      <xsl:value-of select="concat('.@', name())"/>
   </xsl:template>
   <xsl:template match="*" mode="generate-id-from-path" priority="-0.5">
      <xsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
      <xsl:text>.</xsl:text>
      <xsl:choose>
         <xsl:when test="count(. | ../namespace::*) = count(../namespace::*)">
            <xsl:value-of select="concat('.namespace::-',1+count(namespace::*),'-')"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="concat('.',name(),'-',1+count(preceding-sibling::*[name()=name(current())]),'-')"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:template>
   <!--Strip characters--><xsl:template match="text()" priority="-1"/>

   <!--SCHEMA METADATA-->
<xsl:template match="/">
      <xsl:apply-templates select="/" mode="M17"/>
      <xsl:apply-templates select="/" mode="M18"/>
      <xsl:apply-templates select="/" mode="M19"/>
      <xsl:apply-templates select="/" mode="M20"/>
      <xsl:apply-templates select="/" mode="M21"/>
      <xsl:apply-templates select="/" mode="M22"/>
      <xsl:apply-templates select="/" mode="M23"/>
      <xsl:apply-templates select="/" mode="M24"/>
      <xsl:apply-templates select="/" mode="M25"/>
      <xsl:apply-templates select="/" mode="M26"/>
      <xsl:apply-templates select="/" mode="M27"/>
      <xsl:apply-templates select="/" mode="M28"/>
      <xsl:apply-templates select="/" mode="M29"/>
      <xsl:apply-templates select="/" mode="M30"/>
      <xsl:apply-templates select="/" mode="M31"/>
      <xsl:apply-templates select="/" mode="M32"/>
      <xsl:apply-templates select="/" mode="M33"/>
      <xsl:apply-templates select="/" mode="M34"/>
      <xsl:apply-templates select="/" mode="M35"/>
      <xsl:apply-templates select="/" mode="M36"/>
      <xsl:apply-templates select="/" mode="M37"/>
      <xsl:apply-templates select="/" mode="M38"/>
      <xsl:apply-templates select="/" mode="M39"/>
      <xsl:apply-templates select="/" mode="M40"/>
      <xsl:apply-templates select="/" mode="M41"/>
      <xsl:apply-templates select="/" mode="M42"/>
      <xsl:apply-templates select="/" mode="M43"/>
      <xsl:apply-templates select="/" mode="M44"/>
      <xsl:apply-templates select="/" mode="M45"/>
      <xsl:apply-templates select="/" mode="M46"/>
      <xsl:apply-templates select="/" mode="M47"/>
      <xsl:apply-templates select="/" mode="M48"/>
      <xsl:apply-templates select="/" mode="M49"/>
      <xsl:apply-templates select="/" mode="M50"/>
      <xsl:apply-templates select="/" mode="M51"/>
      <xsl:apply-templates select="/" mode="M52"/>
      <xsl:apply-templates select="/" mode="M53"/>
      <xsl:apply-templates select="/" mode="M54"/>
      <xsl:apply-templates select="/" mode="M55"/>
      <xsl:apply-templates select="/" mode="M56"/>
      <xsl:apply-templates select="/" mode="M57"/>
      <xsl:apply-templates select="/" mode="M58"/>
      <xsl:apply-templates select="/" mode="M59"/>
      <xsl:apply-templates select="/" mode="M60"/>
      <xsl:apply-templates select="/" mode="M61"/>
      <xsl:apply-templates select="/" mode="M62"/>
      <xsl:apply-templates select="/" mode="M63"/>
      <xsl:apply-templates select="/" mode="M64"/>
      <xsl:apply-templates select="/" mode="M65"/>
      <xsl:apply-templates select="/" mode="M66"/>
      <xsl:apply-templates select="/" mode="M67"/>
      <xsl:apply-templates select="/" mode="M68"/>
      <xsl:apply-templates select="/" mode="M69"/>
      <xsl:apply-templates select="/" mode="M70"/>
      <xsl:apply-templates select="/" mode="M71"/>
      <xsl:apply-templates select="/" mode="M72"/>
      <xsl:apply-templates select="/" mode="M73"/>
      <xsl:apply-templates select="/" mode="M74"/>
      <xsl:apply-templates select="/" mode="M75"/>
      <xsl:apply-templates select="/" mode="M76"/>
      <xsl:apply-templates select="/" mode="M77"/>
      <xsl:apply-templates select="/" mode="M78"/>
      <xsl:apply-templates select="/" mode="M79"/>
      <xsl:apply-templates select="/" mode="M80"/>
      <xsl:apply-templates select="/" mode="M81"/>
      <xsl:apply-templates select="/" mode="M82"/>
      <xsl:apply-templates select="/" mode="M83"/>
      <xsl:apply-templates select="/" mode="M84"/>
      <xsl:apply-templates select="/" mode="M85"/>
      <xsl:apply-templates select="/" mode="M86"/>
      <xsl:apply-templates select="/" mode="M87"/>
      <xsl:apply-templates select="/" mode="M88"/>
      <xsl:apply-templates select="/" mode="M89"/>
      <xsl:apply-templates select="/" mode="M90"/>
      <xsl:apply-templates select="/" mode="M91"/>
      <xsl:apply-templates select="/" mode="M92"/>
      <xsl:apply-templates select="/" mode="M93"/>
      <xsl:apply-templates select="/" mode="M94"/>
      <xsl:apply-templates select="/" mode="M95"/>
      <xsl:apply-templates select="/" mode="M96"/>
      <xsl:apply-templates select="/" mode="M97"/>
      <xsl:apply-templates select="/" mode="M98"/>
      <xsl:apply-templates select="/" mode="M99"/>
      <xsl:apply-templates select="/" mode="M100"/>
      <xsl:apply-templates select="/" mode="M101"/>
      <xsl:apply-templates select="/" mode="M102"/>
      <xsl:apply-templates select="/" mode="M103"/>
      <xsl:apply-templates select="/" mode="M104"/>
      <xsl:apply-templates select="/" mode="M105"/>
      <xsl:apply-templates select="/" mode="M106"/>
      <xsl:apply-templates select="/" mode="M107"/>
      <xsl:apply-templates select="/" mode="M108"/>
      <xsl:apply-templates select="/" mode="M109"/>
      <xsl:apply-templates select="/" mode="M110"/>
      <xsl:apply-templates select="/" mode="M111"/>
      <xsl:apply-templates select="/" mode="M112"/>
      <xsl:apply-templates select="/" mode="M113"/>
      <xsl:apply-templates select="/" mode="M114"/>
      <xsl:apply-templates select="/" mode="M115"/>
      <xsl:apply-templates select="/" mode="M116"/>
      <xsl:apply-templates select="/" mode="M117"/>
      <xsl:apply-templates select="/" mode="M118"/>
      <xsl:apply-templates select="/" mode="M119"/>
      <xsl:apply-templates select="/" mode="M120"/>
      <xsl:apply-templates select="/" mode="M121"/>
      <xsl:apply-templates select="/" mode="M122"/>
      <xsl:apply-templates select="/" mode="M123"/>
      <xsl:apply-templates select="/" mode="M124"/>
      <xsl:apply-templates select="/" mode="M125"/>
      <xsl:apply-templates select="/" mode="M126"/>
      <xsl:apply-templates select="/" mode="M127"/>
      <xsl:apply-templates select="/" mode="M128"/>
      <xsl:apply-templates select="/" mode="M129"/>
      <xsl:apply-templates select="/" mode="M130"/>
      <xsl:apply-templates select="/" mode="M131"/>
      <xsl:apply-templates select="/" mode="M132"/>
      <xsl:apply-templates select="/" mode="M133"/>
      <xsl:apply-templates select="/" mode="M134"/>
      <xsl:apply-templates select="/" mode="M135"/>
      <xsl:apply-templates select="/" mode="M136"/>
      <xsl:apply-templates select="/" mode="M137"/>
      <xsl:apply-templates select="/" mode="M138"/>
      <xsl:apply-templates select="/" mode="M139"/>
      <xsl:apply-templates select="/" mode="M140"/>
      <xsl:apply-templates select="/" mode="M141"/>
      <xsl:apply-templates select="/" mode="M142"/>
      <xsl:apply-templates select="/" mode="M143"/>
      <xsl:apply-templates select="/" mode="M144"/>
      <xsl:apply-templates select="/" mode="M145"/>
      <xsl:apply-templates select="/" mode="M146"/>
      <xsl:apply-templates select="/" mode="M147"/>
      <xsl:apply-templates select="/" mode="M148"/>
      <xsl:apply-templates select="/" mode="M149"/>
      <xsl:apply-templates select="/" mode="M150"/>
      <xsl:apply-templates select="/" mode="M151"/>
      <xsl:apply-templates select="/" mode="M152"/>
      <xsl:apply-templates select="/" mode="M153"/>
      <xsl:apply-templates select="/" mode="M154"/>
      <xsl:apply-templates select="/" mode="M155"/>
      <xsl:apply-templates select="/" mode="M156"/>
      <xsl:apply-templates select="/" mode="M157"/>
      <xsl:apply-templates select="/" mode="M158"/>
      <xsl:apply-templates select="/" mode="M159"/>
      <xsl:apply-templates select="/" mode="M160"/>
      <xsl:apply-templates select="/" mode="M161"/>
      <xsl:apply-templates select="/" mode="M162"/>
      <xsl:apply-templates select="/" mode="M163"/>
      <xsl:apply-templates select="/" mode="M164"/>
      <xsl:apply-templates select="/" mode="M165"/>
      <xsl:apply-templates select="/" mode="M166"/>
      <xsl:apply-templates select="/" mode="M167"/>
      <xsl:apply-templates select="/" mode="M168"/>
      <xsl:apply-templates select="/" mode="M169"/>
      <xsl:apply-templates select="/" mode="M170"/>
      <xsl:apply-templates select="/" mode="M171"/>
      <xsl:apply-templates select="/" mode="M172"/>
      <xsl:apply-templates select="/" mode="M173"/>
      <xsl:apply-templates select="/" mode="M174"/>
      <xsl:apply-templates select="/" mode="M175"/>
      <xsl:apply-templates select="/" mode="M176"/>
      <xsl:apply-templates select="/" mode="M177"/>
      <xsl:apply-templates select="/" mode="M178"/>
      <xsl:apply-templates select="/" mode="M179"/>
      <xsl:apply-templates select="/" mode="M180"/>
      <xsl:apply-templates select="/" mode="M181"/>
      <xsl:apply-templates select="/" mode="M182"/>
      <xsl:apply-templates select="/" mode="M183"/>
      <xsl:apply-templates select="/" mode="M184"/>
      <xsl:apply-templates select="/" mode="M185"/>
      <xsl:apply-templates select="/" mode="M186"/>
      <xsl:apply-templates select="/" mode="M187"/>
      <xsl:apply-templates select="/" mode="M188"/>
      <xsl:apply-templates select="/" mode="M189"/>
      <xsl:apply-templates select="/" mode="M190"/>
      <xsl:apply-templates select="/" mode="M191"/>
      <xsl:apply-templates select="/" mode="M192"/>
      <xsl:apply-templates select="/" mode="M193"/>
      <xsl:apply-templates select="/" mode="M194"/>
      <xsl:apply-templates select="/" mode="M195"/>
      <xsl:apply-templates select="/" mode="M196"/>
      <xsl:apply-templates select="/" mode="M197"/>
      <xsl:apply-templates select="/" mode="M198"/>
      <xsl:apply-templates select="/" mode="M199"/>
      <xsl:apply-templates select="/" mode="M200"/>
      <xsl:apply-templates select="/" mode="M201"/>
      <xsl:apply-templates select="/" mode="M202"/>
      <xsl:apply-templates select="/" mode="M203"/>
      <xsl:apply-templates select="/" mode="M204"/>
      <xsl:apply-templates select="/" mode="M205"/>
      <xsl:apply-templates select="/" mode="M206"/>
      <xsl:apply-templates select="/" mode="M207"/>
      <xsl:apply-templates select="/" mode="M208"/>
      <xsl:apply-templates select="/" mode="M209"/>
      <xsl:apply-templates select="/" mode="M210"/>
      <xsl:apply-templates select="/" mode="M211"/>
      <xsl:apply-templates select="/" mode="M212"/>
      <xsl:apply-templates select="/" mode="M213"/>
      <xsl:apply-templates select="/" mode="M214"/>
      <xsl:apply-templates select="/" mode="M215"/>
      <xsl:apply-templates select="/" mode="M216"/>
      <xsl:apply-templates select="/" mode="M217"/>
      <xsl:apply-templates select="/" mode="M218"/>
      <xsl:apply-templates select="/" mode="M219"/>
      <xsl:apply-templates select="/" mode="M220"/>
      <xsl:apply-templates select="/" mode="M221"/>
      <xsl:apply-templates select="/" mode="M222"/>
      <xsl:apply-templates select="/" mode="M223"/>
      <xsl:apply-templates select="/" mode="M224"/>
      <xsl:apply-templates select="/" mode="M225"/>
      <xsl:apply-templates select="/" mode="M226"/>
      <xsl:apply-templates select="/" mode="M227"/>
      <xsl:apply-templates select="/" mode="M228"/>
      <xsl:apply-templates select="/" mode="M229"/>
      <xsl:apply-templates select="/" mode="M230"/>
      <xsl:apply-templates select="/" mode="M231"/>
      <xsl:apply-templates select="/" mode="M232"/>
      <xsl:apply-templates select="/" mode="M233"/>
      <xsl:apply-templates select="/" mode="M234"/>
      <xsl:apply-templates select="/" mode="M235"/>
      <xsl:apply-templates select="/" mode="M236"/>
      <xsl:apply-templates select="/" mode="M237"/>
      <xsl:apply-templates select="/" mode="M238"/>
      <xsl:apply-templates select="/" mode="M239"/>
      <xsl:apply-templates select="/" mode="M240"/>
      <xsl:apply-templates select="/" mode="M241"/>
      <xsl:apply-templates select="/" mode="M242"/>
      <xsl:apply-templates select="/" mode="M243"/>
      <xsl:apply-templates select="/" mode="M244"/>
      <xsl:apply-templates select="/" mode="M245"/>
      <xsl:apply-templates select="/" mode="M246"/>
      <xsl:apply-templates select="/" mode="M247"/>
      <xsl:apply-templates select="/" mode="M248"/>
      <xsl:apply-templates select="/" mode="M249"/>
      <xsl:apply-templates select="/" mode="M250"/>
      <xsl:apply-templates select="/" mode="M251"/>
      <xsl:apply-templates select="/" mode="M252"/>
      <xsl:apply-templates select="/" mode="M253"/>
      <xsl:apply-templates select="/" mode="M254"/>
      <xsl:apply-templates select="/" mode="M255"/>
      <xsl:apply-templates select="/" mode="M256"/>
      <xsl:apply-templates select="/" mode="M257"/>
      <xsl:apply-templates select="/" mode="M258"/>
      <xsl:apply-templates select="/" mode="M259"/>
      <xsl:apply-templates select="/" mode="M260"/>
      <xsl:apply-templates select="/" mode="M261"/>
      <xsl:apply-templates select="/" mode="M262"/>
      <xsl:apply-templates select="/" mode="M263"/>
      <xsl:apply-templates select="/" mode="M264"/>
      <xsl:apply-templates select="/" mode="M265"/>
      <xsl:apply-templates select="/" mode="M266"/>
      <xsl:apply-templates select="/" mode="M267"/>
      <xsl:apply-templates select="/" mode="M268"/>
      <xsl:apply-templates select="/" mode="M269"/>
      <xsl:apply-templates select="/" mode="M270"/>
      <xsl:apply-templates select="/" mode="M271"/>
      <xsl:apply-templates select="/" mode="M272"/>
      <xsl:apply-templates select="/" mode="M273"/>
      <xsl:apply-templates select="/" mode="M274"/>
      <xsl:apply-templates select="/" mode="M275"/>
      <xsl:apply-templates select="/" mode="M276"/>
      <xsl:apply-templates select="/" mode="M277"/>
      <xsl:apply-templates select="/" mode="M278"/>
      <xsl:apply-templates select="/" mode="M279"/>
      <xsl:apply-templates select="/" mode="M280"/>
      <xsl:apply-templates select="/" mode="M281"/>
      <xsl:apply-templates select="/" mode="M282"/>
      <xsl:apply-templates select="/" mode="M283"/>
      <xsl:apply-templates select="/" mode="M284"/>
      <xsl:apply-templates select="/" mode="M285"/>
      <xsl:apply-templates select="/" mode="M286"/>
      <xsl:apply-templates select="/" mode="M287"/>
      <xsl:apply-templates select="/" mode="M288"/>
      <xsl:apply-templates select="/" mode="M289"/>
      <xsl:apply-templates select="/" mode="M290"/>
      <xsl:apply-templates select="/" mode="M291"/>
      <xsl:apply-templates select="/" mode="M292"/>
      <xsl:apply-templates select="/" mode="M293"/>
      <xsl:apply-templates select="/" mode="M294"/>
      <xsl:apply-templates select="/" mode="M295"/>
      <xsl:apply-templates select="/" mode="M296"/>
      <xsl:apply-templates select="/" mode="M297"/>
      <xsl:apply-templates select="/" mode="M298"/>
      <xsl:apply-templates select="/" mode="M299"/>
      <xsl:apply-templates select="/" mode="M300"/>
      <xsl:apply-templates select="/" mode="M301"/>
      <xsl:apply-templates select="/" mode="M302"/>
      <xsl:apply-templates select="/" mode="M303"/>
      <xsl:apply-templates select="/" mode="M304"/>
      <xsl:apply-templates select="/" mode="M305"/>
      <xsl:apply-templates select="/" mode="M306"/>
      <xsl:apply-templates select="/" mode="M307"/>
      <xsl:apply-templates select="/" mode="M308"/>
      <xsl:apply-templates select="/" mode="M309"/>
      <xsl:apply-templates select="/" mode="M310"/>
      <xsl:apply-templates select="/" mode="M311"/>
      <xsl:apply-templates select="/" mode="M312"/>
      <xsl:apply-templates select="/" mode="M313"/>
      <xsl:apply-templates select="/" mode="M314"/>
      <xsl:apply-templates select="/" mode="M315"/>
      <xsl:apply-templates select="/" mode="M316"/>
      <xsl:apply-templates select="/" mode="M317"/>
      <xsl:apply-templates select="/" mode="M318"/>
      <xsl:apply-templates select="/" mode="M319"/>
      <xsl:apply-templates select="/" mode="M320"/>
      <xsl:apply-templates select="/" mode="M321"/>
      <xsl:apply-templates select="/" mode="M322"/>
      <xsl:apply-templates select="/" mode="M323"/>
      <xsl:apply-templates select="/" mode="M324"/>
      <xsl:apply-templates select="/" mode="M325"/>
      <xsl:apply-templates select="/" mode="M326"/>
      <xsl:apply-templates select="/" mode="M327"/>
      <xsl:apply-templates select="/" mode="M328"/>
      <xsl:apply-templates select="/" mode="M329"/>
      <xsl:apply-templates select="/" mode="M330"/>
      <xsl:apply-templates select="/" mode="M331"/>
      <xsl:apply-templates select="/" mode="M332"/>
      <xsl:apply-templates select="/" mode="M333"/>
      <xsl:apply-templates select="/" mode="M334"/>
      <xsl:apply-templates select="/" mode="M335"/>
      <xsl:apply-templates select="/" mode="M336"/>
      <xsl:apply-templates select="/" mode="M337"/>
      <xsl:apply-templates select="/" mode="M338"/>
      <xsl:apply-templates select="/" mode="M339"/>
      <xsl:apply-templates select="/" mode="M340"/>
      <xsl:apply-templates select="/" mode="M341"/>
      <xsl:apply-templates select="/" mode="M342"/>
      <xsl:apply-templates select="/" mode="M343"/>
      <xsl:apply-templates select="/" mode="M344"/>
      <xsl:apply-templates select="/" mode="M345"/>
      <xsl:apply-templates select="/" mode="M346"/>
      <xsl:apply-templates select="/" mode="M347"/>
      <xsl:apply-templates select="/" mode="M348"/>
      <xsl:apply-templates select="/" mode="M349"/>
      <xsl:apply-templates select="/" mode="M350"/>
      <xsl:apply-templates select="/" mode="M351"/>
      <xsl:apply-templates select="/" mode="M352"/>
      <xsl:apply-templates select="/" mode="M353"/>
      <xsl:apply-templates select="/" mode="M354"/>
      <xsl:apply-templates select="/" mode="M355"/>
      <xsl:apply-templates select="/" mode="M356"/>
      <xsl:apply-templates select="/" mode="M357"/>
      <xsl:apply-templates select="/" mode="M358"/>
      <xsl:apply-templates select="/" mode="M359"/>
      <xsl:apply-templates select="/" mode="M360"/>
      <xsl:apply-templates select="/" mode="M361"/>
      <xsl:apply-templates select="/" mode="M362"/>
      <xsl:apply-templates select="/" mode="M363"/>
      <xsl:apply-templates select="/" mode="M364"/>
      <xsl:apply-templates select="/" mode="M365"/>
      <xsl:apply-templates select="/" mode="M366"/>
      <xsl:apply-templates select="/" mode="M367"/>
      <xsl:apply-templates select="/" mode="M368"/>
      <xsl:apply-templates select="/" mode="M369"/>
      <xsl:apply-templates select="/" mode="M370"/>
      <xsl:apply-templates select="/" mode="M371"/>
      <xsl:apply-templates select="/" mode="M372"/>
      <xsl:apply-templates select="/" mode="M373"/>
      <xsl:apply-templates select="/" mode="M374"/>
      <xsl:apply-templates select="/" mode="M375"/>
      <xsl:apply-templates select="/" mode="M376"/>
      <xsl:apply-templates select="/" mode="M377"/>
      <xsl:apply-templates select="/" mode="M378"/>
      <xsl:apply-templates select="/" mode="M379"/>
      <xsl:apply-templates select="/" mode="M380"/>
      <xsl:apply-templates select="/" mode="M381"/>
      <xsl:apply-templates select="/" mode="M382"/>
      <xsl:apply-templates select="/" mode="M383"/>
      <xsl:apply-templates select="/" mode="M384"/>
      <xsl:apply-templates select="/" mode="M385"/>
      <xsl:apply-templates select="/" mode="M386"/>
      <xsl:apply-templates select="/" mode="M387"/>
      <xsl:apply-templates select="/" mode="M388"/>
      <xsl:apply-templates select="/" mode="M389"/>
      <xsl:apply-templates select="/" mode="M390"/>
      <xsl:apply-templates select="/" mode="M391"/>
      <xsl:apply-templates select="/" mode="M392"/>
      <xsl:apply-templates select="/" mode="M393"/>
      <xsl:apply-templates select="/" mode="M394"/>
      <xsl:apply-templates select="/" mode="M395"/>
      <xsl:apply-templates select="/" mode="M396"/>
      <xsl:apply-templates select="/" mode="M397"/>
      <xsl:apply-templates select="/" mode="M398"/>
      <xsl:apply-templates select="/" mode="M399"/>
      <xsl:apply-templates select="/" mode="M400"/>
      <xsl:apply-templates select="/" mode="M401"/>
      <xsl:apply-templates select="/" mode="M402"/>
      <xsl:apply-templates select="/" mode="M403"/>
      <xsl:apply-templates select="/" mode="M404"/>
      <xsl:apply-templates select="/" mode="M405"/>
      <xsl:apply-templates select="/" mode="M406"/>
      <xsl:apply-templates select="/" mode="M407"/>
      <xsl:apply-templates select="/" mode="M408"/>
      <xsl:apply-templates select="/" mode="M409"/>
      <xsl:apply-templates select="/" mode="M410"/>
      <xsl:apply-templates select="/" mode="M411"/>
      <xsl:apply-templates select="/" mode="M412"/>
      <xsl:apply-templates select="/" mode="M413"/>
      <xsl:apply-templates select="/" mode="M414"/>
      <xsl:apply-templates select="/" mode="M415"/>
      <xsl:apply-templates select="/" mode="M416"/>
      <xsl:apply-templates select="/" mode="M417"/>
      <xsl:apply-templates select="/" mode="M418"/>
      <xsl:apply-templates select="/" mode="M419"/>
      <xsl:apply-templates select="/" mode="M420"/>
      <xsl:apply-templates select="/" mode="M421"/>
      <xsl:apply-templates select="/" mode="M422"/>
      <xsl:apply-templates select="/" mode="M423"/>
      <xsl:apply-templates select="/" mode="M424"/>
      <xsl:apply-templates select="/" mode="M425"/>
      <xsl:apply-templates select="/" mode="M426"/>
      <xsl:apply-templates select="/" mode="M427"/>
      <xsl:apply-templates select="/" mode="M428"/>
      <xsl:apply-templates select="/" mode="M429"/>
      <xsl:apply-templates select="/" mode="M430"/>
      <xsl:apply-templates select="/" mode="M431"/>
      <xsl:apply-templates select="/" mode="M432"/>
      <xsl:apply-templates select="/" mode="M433"/>
      <xsl:apply-templates select="/" mode="M434"/>
      <xsl:apply-templates select="/" mode="M435"/>
      <xsl:apply-templates select="/" mode="M436"/>
      <xsl:apply-templates select="/" mode="M437"/>
      <xsl:apply-templates select="/" mode="M438"/>
      <xsl:apply-templates select="/" mode="M439"/>
      <xsl:apply-templates select="/" mode="M440"/>
      <xsl:apply-templates select="/" mode="M441"/>
      <xsl:apply-templates select="/" mode="M442"/>
      <xsl:apply-templates select="/" mode="M443"/>
      <xsl:apply-templates select="/" mode="M444"/>
      <xsl:apply-templates select="/" mode="M445"/>
      <xsl:apply-templates select="/" mode="M446"/>
      <xsl:apply-templates select="/" mode="M447"/>
      <xsl:apply-templates select="/" mode="M448"/>
      <xsl:apply-templates select="/" mode="M449"/>
      <xsl:apply-templates select="/" mode="M450"/>
      <xsl:apply-templates select="/" mode="M451"/>
      <xsl:apply-templates select="/" mode="M452"/>
      <xsl:apply-templates select="/" mode="M453"/>
      <xsl:apply-templates select="/" mode="M454"/>
      <xsl:apply-templates select="/" mode="M455"/>
      <xsl:apply-templates select="/" mode="M456"/>
      <xsl:apply-templates select="/" mode="M457"/>
      <xsl:apply-templates select="/" mode="M458"/>
      <xsl:apply-templates select="/" mode="M459"/>
      <xsl:apply-templates select="/" mode="M460"/>
      <xsl:apply-templates select="/" mode="M461"/>
      <xsl:apply-templates select="/" mode="M462"/>
      <xsl:apply-templates select="/" mode="M463"/>
      <xsl:apply-templates select="/" mode="M464"/>
      <xsl:apply-templates select="/" mode="M465"/>
      <xsl:apply-templates select="/" mode="M466"/>
      <xsl:apply-templates select="/" mode="M467"/>
      <xsl:apply-templates select="/" mode="M468"/>
      <xsl:apply-templates select="/" mode="M469"/>
      <xsl:apply-templates select="/" mode="M470"/>
      <xsl:apply-templates select="/" mode="M471"/>
      <xsl:apply-templates select="/" mode="M472"/>
      <xsl:apply-templates select="/" mode="M473"/>
      <xsl:apply-templates select="/" mode="M474"/>
      <xsl:apply-templates select="/" mode="M475"/>
      <xsl:apply-templates select="/" mode="M476"/>
      <xsl:apply-templates select="/" mode="M477"/>
      <xsl:apply-templates select="/" mode="M478"/>
      <xsl:apply-templates select="/" mode="M479"/>
      <xsl:apply-templates select="/" mode="M480"/>
      <xsl:apply-templates select="/" mode="M481"/>
      <xsl:apply-templates select="/" mode="M482"/>
      <xsl:apply-templates select="/" mode="M483"/>
      <xsl:apply-templates select="/" mode="M484"/>
      <xsl:apply-templates select="/" mode="M485"/>
      <xsl:apply-templates select="/" mode="M486"/>
      <xsl:apply-templates select="/" mode="M487"/>
      <xsl:apply-templates select="/" mode="M488"/>
      <xsl:apply-templates select="/" mode="M489"/>
      <xsl:apply-templates select="/" mode="M490"/>
      <xsl:apply-templates select="/" mode="M491"/>
      <xsl:apply-templates select="/" mode="M492"/>
      <xsl:apply-templates select="/" mode="M493"/>
      <xsl:apply-templates select="/" mode="M494"/>
      <xsl:apply-templates select="/" mode="M495"/>
      <xsl:apply-templates select="/" mode="M496"/>
      <xsl:apply-templates select="/" mode="M497"/>
      <xsl:apply-templates select="/" mode="M498"/>
      <xsl:apply-templates select="/" mode="M499"/>
      <xsl:apply-templates select="/" mode="M500"/>
      <xsl:apply-templates select="/" mode="M501"/>
      <xsl:apply-templates select="/" mode="M502"/>
      <xsl:apply-templates select="/" mode="M503"/>
      <xsl:apply-templates select="/" mode="M504"/>
      <xsl:apply-templates select="/" mode="M505"/>
      <xsl:apply-templates select="/" mode="M506"/>
      <xsl:apply-templates select="/" mode="M507"/>
      <xsl:apply-templates select="/" mode="M508"/>
      <xsl:apply-templates select="/" mode="M509"/>
      <xsl:apply-templates select="/" mode="M510"/>
      <xsl:apply-templates select="/" mode="M511"/>
      <xsl:apply-templates select="/" mode="M512"/>
      <xsl:apply-templates select="/" mode="M513"/>
      <xsl:apply-templates select="/" mode="M514"/>
      <xsl:apply-templates select="/" mode="M515"/>
      <xsl:apply-templates select="/" mode="M516"/>
      <xsl:apply-templates select="/" mode="M517"/>
      <xsl:apply-templates select="/" mode="M518"/>
      <xsl:apply-templates select="/" mode="M519"/>
      <xsl:apply-templates select="/" mode="M520"/>
      <xsl:apply-templates select="/" mode="M521"/>
      <xsl:apply-templates select="/" mode="M522"/>
      <xsl:apply-templates select="/" mode="M523"/>
      <xsl:apply-templates select="/" mode="M524"/>
      <xsl:apply-templates select="/" mode="M525"/>
      <xsl:apply-templates select="/" mode="M526"/>
      <xsl:apply-templates select="/" mode="M527"/>
      <xsl:apply-templates select="/" mode="M528"/>
      <xsl:apply-templates select="/" mode="M529"/>
      <xsl:apply-templates select="/" mode="M530"/>
      <xsl:apply-templates select="/" mode="M531"/>
      <xsl:apply-templates select="/" mode="M532"/>
      <xsl:apply-templates select="/" mode="M533"/>
      <xsl:apply-templates select="/" mode="M534"/>
      <xsl:apply-templates select="/" mode="M535"/>
      <xsl:apply-templates select="/" mode="M536"/>
      <xsl:apply-templates select="/" mode="M537"/>
      <xsl:apply-templates select="/" mode="M538"/>
      <xsl:apply-templates select="/" mode="M539"/>
      <xsl:apply-templates select="/" mode="M540"/>
      <xsl:apply-templates select="/" mode="M541"/>
      <xsl:apply-templates select="/" mode="M542"/>
      <xsl:apply-templates select="/" mode="M543"/>
      <xsl:apply-templates select="/" mode="M544"/>
      <xsl:apply-templates select="/" mode="M545"/>
      <xsl:apply-templates select="/" mode="M546"/>
      <xsl:apply-templates select="/" mode="M547"/>
      <xsl:apply-templates select="/" mode="M548"/>
      <xsl:apply-templates select="/" mode="M549"/>
      <xsl:apply-templates select="/" mode="M550"/>
      <xsl:apply-templates select="/" mode="M551"/>
      <xsl:apply-templates select="/" mode="M552"/>
      <xsl:apply-templates select="/" mode="M553"/>
      <xsl:apply-templates select="/" mode="M554"/>
      <xsl:apply-templates select="/" mode="M555"/>
      <xsl:apply-templates select="/" mode="M556"/>
      <xsl:apply-templates select="/" mode="M557"/>
      <xsl:apply-templates select="/" mode="M558"/>
      <xsl:apply-templates select="/" mode="M559"/>
      <xsl:apply-templates select="/" mode="M560"/>
      <xsl:apply-templates select="/" mode="M561"/>
      <xsl:apply-templates select="/" mode="M562"/>
      <xsl:apply-templates select="/" mode="M563"/>
      <xsl:apply-templates select="/" mode="M564"/>
      <xsl:apply-templates select="/" mode="M565"/>
      <xsl:apply-templates select="/" mode="M566"/>
      <xsl:apply-templates select="/" mode="M567"/>
      <xsl:apply-templates select="/" mode="M568"/>
      <xsl:apply-templates select="/" mode="M569"/>
      <xsl:apply-templates select="/" mode="M570"/>
      <xsl:apply-templates select="/" mode="M571"/>
      <xsl:apply-templates select="/" mode="M572"/>
      <xsl:apply-templates select="/" mode="M573"/>
      <xsl:apply-templates select="/" mode="M574"/>
      <xsl:apply-templates select="/" mode="M575"/>
      <xsl:apply-templates select="/" mode="M576"/>
      <xsl:apply-templates select="/" mode="M577"/>
      <xsl:apply-templates select="/" mode="M578"/>
      <xsl:apply-templates select="/" mode="M579"/>
      <xsl:apply-templates select="/" mode="M580"/>
      <xsl:apply-templates select="/" mode="M581"/>
      <xsl:apply-templates select="/" mode="M582"/>
      <xsl:apply-templates select="/" mode="M583"/>
      <xsl:apply-templates select="/" mode="M584"/>
      <xsl:apply-templates select="/" mode="M585"/>
      <xsl:apply-templates select="/" mode="M586"/>
      <xsl:apply-templates select="/" mode="M587"/>
      <xsl:apply-templates select="/" mode="M588"/>
      <xsl:apply-templates select="/" mode="M589"/>
      <xsl:apply-templates select="/" mode="M590"/>
      <xsl:apply-templates select="/" mode="M591"/>
      <xsl:apply-templates select="/" mode="M592"/>
      <xsl:apply-templates select="/" mode="M593"/>
      <xsl:apply-templates select="/" mode="M594"/>
      <xsl:apply-templates select="/" mode="M595"/>
      <xsl:apply-templates select="/" mode="M596"/>
      <xsl:apply-templates select="/" mode="M597"/>
      <xsl:apply-templates select="/" mode="M598"/>
      <xsl:apply-templates select="/" mode="M599"/>
      <xsl:apply-templates select="/" mode="M600"/>
      <xsl:apply-templates select="/" mode="M601"/>
      <xsl:apply-templates select="/" mode="M602"/>
      <xsl:apply-templates select="/" mode="M603"/>
      <xsl:apply-templates select="/" mode="M604"/>
      <xsl:apply-templates select="/" mode="M605"/>
      <xsl:apply-templates select="/" mode="M606"/>
      <xsl:apply-templates select="/" mode="M607"/>
      <xsl:apply-templates select="/" mode="M608"/>
      <xsl:apply-templates select="/" mode="M609"/>
      <xsl:apply-templates select="/" mode="M610"/>
      <xsl:apply-templates select="/" mode="M611"/>
      <xsl:apply-templates select="/" mode="M612"/>
      <xsl:apply-templates select="/" mode="M613"/>
      <xsl:apply-templates select="/" mode="M614"/>
      <xsl:apply-templates select="/" mode="M615"/>
      <xsl:apply-templates select="/" mode="M616"/>
      <xsl:apply-templates select="/" mode="M617"/>
      <xsl:apply-templates select="/" mode="M618"/>
      <xsl:apply-templates select="/" mode="M619"/>
      <xsl:apply-templates select="/" mode="M620"/>
      <xsl:apply-templates select="/" mode="M621"/>
      <xsl:apply-templates select="/" mode="M622"/>
      <xsl:apply-templates select="/" mode="M623"/>
      <xsl:apply-templates select="/" mode="M624"/>
      <xsl:apply-templates select="/" mode="M625"/>
      <xsl:apply-templates select="/" mode="M626"/>
      <xsl:apply-templates select="/" mode="M627"/>
      <xsl:apply-templates select="/" mode="M628"/>
      <xsl:apply-templates select="/" mode="M629"/>
      <xsl:apply-templates select="/" mode="M630"/>
      <xsl:apply-templates select="/" mode="M631"/>
      <xsl:apply-templates select="/" mode="M632"/>
      <xsl:apply-templates select="/" mode="M633"/>
      <xsl:apply-templates select="/" mode="M634"/>
      <xsl:apply-templates select="/" mode="M635"/>
      <xsl:apply-templates select="/" mode="M636"/>
      <xsl:apply-templates select="/" mode="M637"/>
      <xsl:apply-templates select="/" mode="M638"/>
      <xsl:apply-templates select="/" mode="M639"/>
      <xsl:apply-templates select="/" mode="M640"/>
      <xsl:apply-templates select="/" mode="M641"/>
      <xsl:apply-templates select="/" mode="M642"/>
      <xsl:apply-templates select="/" mode="M643"/>
      <xsl:apply-templates select="/" mode="M644"/>
      <xsl:apply-templates select="/" mode="M645"/>
      <xsl:apply-templates select="/" mode="M646"/>
      <xsl:apply-templates select="/" mode="M647"/>
      <xsl:apply-templates select="/" mode="M648"/>
      <xsl:apply-templates select="/" mode="M649"/>
      <xsl:apply-templates select="/" mode="M650"/>
      <xsl:apply-templates select="/" mode="M651"/>
      <xsl:apply-templates select="/" mode="M652"/>
      <xsl:apply-templates select="/" mode="M653"/>
      <xsl:apply-templates select="/" mode="M654"/>
      <xsl:apply-templates select="/" mode="M655"/>
      <xsl:apply-templates select="/" mode="M656"/>
      <xsl:apply-templates select="/" mode="M657"/>
      <xsl:apply-templates select="/" mode="M658"/>
      <xsl:apply-templates select="/" mode="M659"/>
      <xsl:apply-templates select="/" mode="M660"/>
      <xsl:apply-templates select="/" mode="M661"/>
      <xsl:apply-templates select="/" mode="M662"/>
      <xsl:apply-templates select="/" mode="M663"/>
      <xsl:apply-templates select="/" mode="M664"/>
      <xsl:apply-templates select="/" mode="M665"/>
      <xsl:apply-templates select="/" mode="M666"/>
      <xsl:apply-templates select="/" mode="M667"/>
      <xsl:apply-templates select="/" mode="M668"/>
      <xsl:apply-templates select="/" mode="M669"/>
      <xsl:apply-templates select="/" mode="M670"/>
      <xsl:apply-templates select="/" mode="M671"/>
      <xsl:apply-templates select="/" mode="M672"/>
      <xsl:apply-templates select="/" mode="M673"/>
      <xsl:apply-templates select="/" mode="M674"/>
      <xsl:apply-templates select="/" mode="M675"/>
      <xsl:apply-templates select="/" mode="M676"/>
      <xsl:apply-templates select="/" mode="M677"/>
      <xsl:apply-templates select="/" mode="M678"/>
      <xsl:apply-templates select="/" mode="M679"/>
      <xsl:apply-templates select="/" mode="M680"/>
      <xsl:apply-templates select="/" mode="M681"/>
      <xsl:apply-templates select="/" mode="M682"/>
      <xsl:apply-templates select="/" mode="M683"/>
      <xsl:apply-templates select="/" mode="M684"/>
      <xsl:apply-templates select="/" mode="M685"/>
      <xsl:apply-templates select="/" mode="M686"/>
      <xsl:apply-templates select="/" mode="M687"/>
      <xsl:apply-templates select="/" mode="M688"/>
      <xsl:apply-templates select="/" mode="M689"/>
      <xsl:apply-templates select="/" mode="M690"/>
      <xsl:apply-templates select="/" mode="M691"/>
      <xsl:apply-templates select="/" mode="M692"/>
      <xsl:apply-templates select="/" mode="M693"/>
      <xsl:apply-templates select="/" mode="M694"/>
      <xsl:apply-templates select="/" mode="M695"/>
      <xsl:apply-templates select="/" mode="M696"/>
      <xsl:apply-templates select="/" mode="M697"/>
      <xsl:apply-templates select="/" mode="M698"/>
      <xsl:apply-templates select="/" mode="M699"/>
      <xsl:apply-templates select="/" mode="M700"/>
      <xsl:apply-templates select="/" mode="M701"/>
      <xsl:apply-templates select="/" mode="M702"/>
      <xsl:apply-templates select="/" mode="M703"/>
      <xsl:apply-templates select="/" mode="M704"/>
      <xsl:apply-templates select="/" mode="M705"/>
      <xsl:apply-templates select="/" mode="M706"/>
      <xsl:apply-templates select="/" mode="M707"/>
      <xsl:apply-templates select="/" mode="M708"/>
      <xsl:apply-templates select="/" mode="M709"/>
      <xsl:apply-templates select="/" mode="M710"/>
      <xsl:apply-templates select="/" mode="M711"/>
      <xsl:apply-templates select="/" mode="M712"/>
      <xsl:apply-templates select="/" mode="M713"/>
      <xsl:apply-templates select="/" mode="M714"/>
      <xsl:apply-templates select="/" mode="M715"/>
      <xsl:apply-templates select="/" mode="M716"/>
      <xsl:apply-templates select="/" mode="M717"/>
      <xsl:apply-templates select="/" mode="M718"/>
      <xsl:apply-templates select="/" mode="M719"/>
      <xsl:apply-templates select="/" mode="M720"/>
      <xsl:apply-templates select="/" mode="M721"/>
      <xsl:apply-templates select="/" mode="M722"/>
      <xsl:apply-templates select="/" mode="M723"/>
      <xsl:apply-templates select="/" mode="M724"/>
      <xsl:apply-templates select="/" mode="M725"/>
      <xsl:apply-templates select="/" mode="M726"/>
      <xsl:apply-templates select="/" mode="M727"/>
      <xsl:apply-templates select="/" mode="M728"/>
      <xsl:apply-templates select="/" mode="M729"/>
      <xsl:apply-templates select="/" mode="M730"/>
      <xsl:apply-templates select="/" mode="M731"/>
      <xsl:apply-templates select="/" mode="M732"/>
      <xsl:apply-templates select="/" mode="M733"/>
      <xsl:apply-templates select="/" mode="M734"/>
      <xsl:apply-templates select="/" mode="M735"/>
      <xsl:apply-templates select="/" mode="M736"/>
      <xsl:apply-templates select="/" mode="M737"/>
      <xsl:apply-templates select="/" mode="M738"/>
      <xsl:apply-templates select="/" mode="M739"/>
      <xsl:apply-templates select="/" mode="M740"/>
      <xsl:apply-templates select="/" mode="M741"/>
      <xsl:apply-templates select="/" mode="M742"/>
      <xsl:apply-templates select="/" mode="M743"/>
      <xsl:apply-templates select="/" mode="M744"/>
      <xsl:apply-templates select="/" mode="M745"/>
      <xsl:apply-templates select="/" mode="M746"/>
      <xsl:apply-templates select="/" mode="M747"/>
      <xsl:apply-templates select="/" mode="M748"/>
      <xsl:apply-templates select="/" mode="M749"/>
      <xsl:apply-templates select="/" mode="M750"/>
      <xsl:apply-templates select="/" mode="M751"/>
      <xsl:apply-templates select="/" mode="M752"/>
      <xsl:apply-templates select="/" mode="M753"/>
      <xsl:apply-templates select="/" mode="M754"/>
      <xsl:apply-templates select="/" mode="M755"/>
      <xsl:apply-templates select="/" mode="M756"/>
      <xsl:apply-templates select="/" mode="M757"/>
      <xsl:apply-templates select="/" mode="M758"/>
      <xsl:apply-templates select="/" mode="M759"/>
      <xsl:apply-templates select="/" mode="M760"/>
      <xsl:apply-templates select="/" mode="M761"/>
      <xsl:apply-templates select="/" mode="M762"/>
      <xsl:apply-templates select="/" mode="M763"/>
      <xsl:apply-templates select="/" mode="M764"/>
      <xsl:apply-templates select="/" mode="M765"/>
      <xsl:apply-templates select="/" mode="M766"/>
      <xsl:apply-templates select="/" mode="M767"/>
      <xsl:apply-templates select="/" mode="M768"/>
      <xsl:apply-templates select="/" mode="M769"/>
      <xsl:apply-templates select="/" mode="M770"/>
      <xsl:apply-templates select="/" mode="M771"/>
      <xsl:apply-templates select="/" mode="M772"/>
      <xsl:apply-templates select="/" mode="M773"/>
      <xsl:apply-templates select="/" mode="M774"/>
      <xsl:apply-templates select="/" mode="M775"/>
      <xsl:apply-templates select="/" mode="M776"/>
      <xsl:apply-templates select="/" mode="M777"/>
      <xsl:apply-templates select="/" mode="M778"/>
      <xsl:apply-templates select="/" mode="M779"/>
      <xsl:apply-templates select="/" mode="M780"/>
      <xsl:apply-templates select="/" mode="M781"/>
      <xsl:apply-templates select="/" mode="M782"/>
      <xsl:apply-templates select="/" mode="M783"/>
      <xsl:apply-templates select="/" mode="M784"/>
      <xsl:apply-templates select="/" mode="M785"/>
      <xsl:apply-templates select="/" mode="M786"/>
      <xsl:apply-templates select="/" mode="M787"/>
      <xsl:apply-templates select="/" mode="M788"/>
      <xsl:apply-templates select="/" mode="M789"/>
      <xsl:apply-templates select="/" mode="M790"/>
      <xsl:apply-templates select="/" mode="M791"/>
      <xsl:apply-templates select="/" mode="M792"/>
      <xsl:apply-templates select="/" mode="M793"/>
      <xsl:apply-templates select="/" mode="M794"/>
      <xsl:apply-templates select="/" mode="M795"/>
      <xsl:apply-templates select="/" mode="M796"/>
      <xsl:apply-templates select="/" mode="M797"/>
      <xsl:apply-templates select="/" mode="M798"/>
      <xsl:apply-templates select="/" mode="M799"/>
      <xsl:apply-templates select="/" mode="M800"/>
      <xsl:apply-templates select="/" mode="M801"/>
      <xsl:apply-templates select="/" mode="M802"/>
      <xsl:apply-templates select="/" mode="M803"/>
      <xsl:apply-templates select="/" mode="M804"/>
      <xsl:apply-templates select="/" mode="M805"/>
      <xsl:apply-templates select="/" mode="M806"/>
      <xsl:apply-templates select="/" mode="M807"/>
      <xsl:apply-templates select="/" mode="M808"/>
      <xsl:apply-templates select="/" mode="M809"/>
      <xsl:apply-templates select="/" mode="M810"/>
      <xsl:apply-templates select="/" mode="M811"/>
      <xsl:apply-templates select="/" mode="M812"/>
      <xsl:apply-templates select="/" mode="M813"/>
      <xsl:apply-templates select="/" mode="M814"/>
      <xsl:apply-templates select="/" mode="M815"/>
      <xsl:apply-templates select="/" mode="M816"/>
      <xsl:apply-templates select="/" mode="M817"/>
      <xsl:apply-templates select="/" mode="M818"/>
      <xsl:apply-templates select="/" mode="M819"/>
      <xsl:apply-templates select="/" mode="M820"/>
      <xsl:apply-templates select="/" mode="M821"/>
      <xsl:apply-templates select="/" mode="M822"/>
      <xsl:apply-templates select="/" mode="M823"/>
      <xsl:apply-templates select="/" mode="M824"/>
      <xsl:apply-templates select="/" mode="M825"/>
      <xsl:apply-templates select="/" mode="M826"/>
      <xsl:apply-templates select="/" mode="M827"/>
      <xsl:apply-templates select="/" mode="M828"/>
      <xsl:apply-templates select="/" mode="M829"/>
      <xsl:apply-templates select="/" mode="M830"/>
      <xsl:apply-templates select="/" mode="M831"/>
      <xsl:apply-templates select="/" mode="M832"/>
      <xsl:apply-templates select="/" mode="M833"/>
      <xsl:apply-templates select="/" mode="M834"/>
      <xsl:apply-templates select="/" mode="M835"/>
      <xsl:apply-templates select="/" mode="M836"/>
      <xsl:apply-templates select="/" mode="M837"/>
      <xsl:apply-templates select="/" mode="M838"/>
      <xsl:apply-templates select="/" mode="M839"/>
      <xsl:apply-templates select="/" mode="M840"/>
      <xsl:apply-templates select="/" mode="M841"/>
      <xsl:apply-templates select="/" mode="M842"/>
      <xsl:apply-templates select="/" mode="M843"/>
      <xsl:apply-templates select="/" mode="M844"/>
      <xsl:apply-templates select="/" mode="M845"/>
      <xsl:apply-templates select="/" mode="M846"/>
      <xsl:apply-templates select="/" mode="M847"/>
      <xsl:apply-templates select="/" mode="M848"/>
      <xsl:apply-templates select="/" mode="M849"/>
      <xsl:apply-templates select="/" mode="M850"/>
      <xsl:apply-templates select="/" mode="M851"/>
      <xsl:apply-templates select="/" mode="M852"/>
      <xsl:apply-templates select="/" mode="M853"/>
      <xsl:apply-templates select="/" mode="M854"/>
      <xsl:apply-templates select="/" mode="M855"/>
      <xsl:apply-templates select="/" mode="M856"/>
      <xsl:apply-templates select="/" mode="M857"/>
      <xsl:apply-templates select="/" mode="M858"/>
      <xsl:apply-templates select="/" mode="M859"/>
      <xsl:apply-templates select="/" mode="M860"/>
      <xsl:apply-templates select="/" mode="M861"/>
      <xsl:apply-templates select="/" mode="M862"/>
      <xsl:apply-templates select="/" mode="M863"/>
      <xsl:apply-templates select="/" mode="M864"/>
      <xsl:apply-templates select="/" mode="M865"/>
      <xsl:apply-templates select="/" mode="M866"/>
      <xsl:apply-templates select="/" mode="M867"/>
      <xsl:apply-templates select="/" mode="M868"/>
      <xsl:apply-templates select="/" mode="M869"/>
      <xsl:apply-templates select="/" mode="M870"/>
      <xsl:apply-templates select="/" mode="M871"/>
      <xsl:apply-templates select="/" mode="M872"/>
      <xsl:apply-templates select="/" mode="M873"/>
      <xsl:apply-templates select="/" mode="M874"/>
      <xsl:apply-templates select="/" mode="M875"/>
      <xsl:apply-templates select="/" mode="M876"/>
      <xsl:apply-templates select="/" mode="M877"/>
      <xsl:apply-templates select="/" mode="M878"/>
      <xsl:apply-templates select="/" mode="M879"/>
      <xsl:apply-templates select="/" mode="M880"/>
      <xsl:apply-templates select="/" mode="M881"/>
      <xsl:apply-templates select="/" mode="M882"/>
      <xsl:apply-templates select="/" mode="M883"/>
      <xsl:apply-templates select="/" mode="M884"/>
      <xsl:apply-templates select="/" mode="M885"/>
      <xsl:apply-templates select="/" mode="M886"/>
      <xsl:apply-templates select="/" mode="M887"/>
      <xsl:apply-templates select="/" mode="M888"/>
      <xsl:apply-templates select="/" mode="M889"/>
      <xsl:apply-templates select="/" mode="M890"/>
      <xsl:apply-templates select="/" mode="M891"/>
      <xsl:apply-templates select="/" mode="M892"/>
      <xsl:apply-templates select="/" mode="M893"/>
      <xsl:apply-templates select="/" mode="M894"/>
      <xsl:apply-templates select="/" mode="M895"/>
      <xsl:apply-templates select="/" mode="M896"/>
      <xsl:apply-templates select="/" mode="M897"/>
      <xsl:apply-templates select="/" mode="M898"/>
      <xsl:apply-templates select="/" mode="M899"/>
      <xsl:apply-templates select="/" mode="M900"/>
      <xsl:apply-templates select="/" mode="M901"/>
      <xsl:apply-templates select="/" mode="M902"/>
      <xsl:apply-templates select="/" mode="M903"/>
      <xsl:apply-templates select="/" mode="M904"/>
      <xsl:apply-templates select="/" mode="M905"/>
      <xsl:apply-templates select="/" mode="M906"/>
      <xsl:apply-templates select="/" mode="M907"/>
      <xsl:apply-templates select="/" mode="M908"/>
      <xsl:apply-templates select="/" mode="M909"/>
      <xsl:apply-templates select="/" mode="M910"/>
      <xsl:apply-templates select="/" mode="M911"/>
   </xsl:template>

   <!--SCHEMATRON PATTERNS-->


<!--PATTERN empty_def_doc-->


	<!--RULE -->
<xsl:template match="oval-def:oval_definitions" priority="4000" mode="M17">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="oval-def:definitions or oval-def:tests or oval-def:objects or oval-def:states or oval-def:variables"/>
         <xsl:otherwise>A valid OVAL Definition document must contain at least one definitions, tests, objects, states, or variables element. The optional definitions, tests, objects, states, and variables sections define the specific characteristics that should be evaluated on a system to determine the truth values of the OVAL Definition Document. To be valid though, at least one definitions, tests, objects, states, or variables element must be present.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M17"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M17"/>
   <xsl:template match="@*|node()" priority="-2" mode="M17">
      <xsl:apply-templates select="@*|node()" mode="M17"/>
   </xsl:template>

   <!--PATTERN required_criteria-->


	<!--RULE -->
<xsl:template match="/oval-def:oval_definitions/oval-def:definitions/oval-def:definition[@deprecated='false' or not(@deprecated)]"
                 priority="4000"
                 mode="M18">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="oval-def:criteria"/>
         <xsl:otherwise>A valid OVAL Definition document must contain a criteria unless the definition is a deprecated definition.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M18"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M18"/>
   <xsl:template match="@*|node()" priority="-2" mode="M18">
      <xsl:apply-templates select="@*|node()" mode="M18"/>
   </xsl:template>

   <!--PATTERN test_type-->


	<!--RULE -->
<xsl:template match="/oval-def:oval_definitions/oval-def:tests/*[@check_existence='none_exist']"
                 priority="4000"
                 mode="M19">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(*[name()='state'])"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="@id"/>
            <xsl:text/> - No state should be referenced when check_existence has a value of 'none_exist'.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M19"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M19"/>
   <xsl:template match="@*|node()" priority="-2" mode="M19">
      <xsl:apply-templates select="@*|node()" mode="M19"/>
   </xsl:template>

   <!--PATTERN setobjref-->


	<!--RULE -->
<xsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:object_reference"
                 priority="4000"
                 mode="M20">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="name(./../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../../@id"/>
            <xsl:text/> - Each object referenced by the set must be of the same type as parent object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M20"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:object_reference"
                 priority="3999"
                 mode="M20">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="name(./../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../../../@id"/>
            <xsl:text/> - Each object referenced by the set must be of the same type as parent object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M20"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="/oval-def:oval_definitions/oval-def:objects/*/oval-def:set/oval-def:set/oval-def:set/oval-def:object_reference"
                 priority="3998"
                 mode="M20">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="name(./../../../..) = name(/oval-def:oval_definitions/oval-def:objects/*[@id=current()])"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../../../../@id"/>
            <xsl:text/> - Each object referenced by the set must be of the same type as parent object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M20"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M20"/>
   <xsl:template match="@*|node()" priority="-2" mode="M20">
      <xsl:apply-templates select="@*|node()" mode="M20"/>
   </xsl:template>

   <!--PATTERN value-->


	<!--RULE -->
<xsl:template match="oval-def:constant_variable/oval-def:value" priority="4000" mode="M21">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test=".!=''"/>
         <xsl:otherwise>The value element of the constant_variable <xsl:text/>
            <xsl:value-of select="../../@id"/>
            <xsl:text/> can not be empty.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M21"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M21"/>
   <xsl:template match="@*|node()" priority="-2" mode="M21">
      <xsl:apply-templates select="@*|node()" mode="M21"/>
   </xsl:template>

   <!--PATTERN literal_component-->


	<!--RULE -->
<xsl:template match="oval-def:literal_component" priority="4000" mode="M22">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test=".!=''"/>
         <xsl:otherwise>The literal_component of variable <xsl:text/>
            <xsl:value-of select="../../@id"/>
            <xsl:text/> can not be empty.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M22"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M22"/>
   <xsl:template match="@*|node()" priority="-2" mode="M22">
      <xsl:apply-templates select="@*|node()" mode="M22"/>
   </xsl:template>

   <!--PATTERN item_field-->


	<!--RULE -->
<xsl:template match="oval-def:object_component" priority="4000" mode="M23">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@item_field!=''"/>
         <xsl:otherwise>The item_field attribute of an object_component of variable <xsl:text/>
            <xsl:value-of select="../../@id"/>
            <xsl:text/> can not be empty.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M23"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M23"/>
   <xsl:template match="@*|node()" priority="-2" mode="M23">
      <xsl:apply-templates select="@*|node()" mode="M23"/>
   </xsl:template>

   <!--PATTERN entityrules-->


	<!--RULE -->
<xsl:template match="oval-def:objects/*/*|oval-def:states/*/*" priority="4000" mode="M24">
      <xsl:variable name="var_ref" select="@var_ref"/>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="((not(@datatype) and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='case insensitive equals' or @operation='case insensitive not equal' or @operation='pattern match')) or (@datatype='binary' and (not(@operation) or @operation='equals' or @operation='not equal')) or (@datatype='boolean' and (not(@operation) or @operation='equals' or @operation='not equal')) or (@datatype='evr_string' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='fileset_revision' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='float' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='ios_version' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')) or (@datatype='int' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='bitwise and' or @operation='bitwise or')) or (@datatype='string' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match')) or (@datatype='version' and (not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal')))"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the supplied operation attribute for the <xsl:text/>
            <xsl:value-of select="name()"/>
            <xsl:text/> entity is not valid given a datatype of '<xsl:text/>
            <xsl:value-of select="@datatype"/>
            <xsl:text/>'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@var_ref) or .=''"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - a var-ref has been supplied for the <xsl:text/>
            <xsl:value-of select="name()"/>
            <xsl:text/> entity so no value should be provided<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (@var_ref) then (if (@datatype) then (@datatype=/oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype) else ('string' = /oval-def:oval_definitions/oval-def:variables/*[@id=$var_ref]/@datatype)) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="$var_ref"/>
            <xsl:text/> - inconsistent datatype between the variable and an associated var_ref<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M24"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M24"/>
   <xsl:template match="@*|node()" priority="-2" mode="M24">
      <xsl:apply-templates select="@*|node()" mode="M24"/>
   </xsl:template>

   <!--PATTERN famtst-->


	<!--RULE -->
<xsl:template match="ind-def:family_test/ind-def:object" priority="4000" mode="M25">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:family_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a family_test must reference a family_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M25"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:family_test/ind-def:state" priority="3999" mode="M25">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:family_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a family_test must reference a family_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M25"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M25"/>
   <xsl:template match="@*|node()" priority="-2" mode="M25">
      <xsl:apply-templates select="@*|node()" mode="M25"/>
   </xsl:template>

   <!--PATTERN famstefamily-->


	<!--RULE -->
<xsl:template match="ind-def:family_state/ind-def:family" priority="4000" mode="M26">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the family entity of a family_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M26"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M26"/>
   <xsl:template match="@*|node()" priority="-2" mode="M26">
      <xsl:apply-templates select="@*|node()" mode="M26"/>
   </xsl:template>

   <!--PATTERN md5tst-->


	<!--RULE -->
<xsl:template match="ind-def:filemd5_test/ind-def:object" priority="4000" mode="M27">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:filemd5_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a filemd5_test must reference a filemd5_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M27"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:filemd5_test/ind-def:state" priority="3999" mode="M27">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:filemd5_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a filemd5_test must reference a filemd5_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M27"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M27"/>
   <xsl:template match="@*|node()" priority="-2" mode="M27">
      <xsl:apply-templates select="@*|node()" mode="M27"/>
   </xsl:template>

   <!--PATTERN md5objpath-->


	<!--RULE -->
<xsl:template match="ind-def:filemd5_object/ind-def:path" priority="4000" mode="M28">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a filemd5_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M28"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M28"/>
   <xsl:template match="@*|node()" priority="-2" mode="M28">
      <xsl:apply-templates select="@*|node()" mode="M28"/>
   </xsl:template>

   <!--PATTERN md5objfilename-->


	<!--RULE -->
<xsl:template match="ind-def:filemd5_object/ind-def:filename" priority="4000" mode="M29">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a filemd5_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M29"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M29"/>
   <xsl:template match="@*|node()" priority="-2" mode="M29">
      <xsl:apply-templates select="@*|node()" mode="M29"/>
   </xsl:template>

   <!--PATTERN md5stepath-->


	<!--RULE -->
<xsl:template match="ind-def:filemd5_state/ind-def:path" priority="4000" mode="M30">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a filemd5_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M30"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M30"/>
   <xsl:template match="@*|node()" priority="-2" mode="M30">
      <xsl:apply-templates select="@*|node()" mode="M30"/>
   </xsl:template>

   <!--PATTERN md5stefilename-->


	<!--RULE -->
<xsl:template match="ind-def:filemd5_state/ind-def:filename" priority="4000" mode="M31">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a filemd5_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M31"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M31"/>
   <xsl:template match="@*|node()" priority="-2" mode="M31">
      <xsl:apply-templates select="@*|node()" mode="M31"/>
   </xsl:template>

   <!--PATTERN md5stemd5-->


	<!--RULE -->
<xsl:template match="ind-def:filemd5_state/ind-def:md5" priority="4000" mode="M32">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the md5 entity of a filemd5_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M32"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M32"/>
   <xsl:template match="@*|node()" priority="-2" mode="M32">
      <xsl:apply-templates select="@*|node()" mode="M32"/>
   </xsl:template>

   <!--PATTERN hashtst-->


	<!--RULE -->
<xsl:template match="ind-def:filehash_test/ind-def:object" priority="4000" mode="M33">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:filehash_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a filehash_test must reference a filesha1_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M33"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:filehash_test/ind-def:state" priority="3999" mode="M33">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:filehash_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a filehash_test must reference a filesha1_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M33"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M33"/>
   <xsl:template match="@*|node()" priority="-2" mode="M33">
      <xsl:apply-templates select="@*|node()" mode="M33"/>
   </xsl:template>

   <!--PATTERN hashobjpath-->


	<!--RULE -->
<xsl:template match="ind-def:filehash_object/ind-def:path" priority="4000" mode="M34">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a filehash_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M34"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M34"/>
   <xsl:template match="@*|node()" priority="-2" mode="M34">
      <xsl:apply-templates select="@*|node()" mode="M34"/>
   </xsl:template>

   <!--PATTERN hashobjfilename-->


	<!--RULE -->
<xsl:template match="ind-def:filehash_object/ind-def:filename" priority="4000" mode="M35">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a filehash_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M35"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M35"/>
   <xsl:template match="@*|node()" priority="-2" mode="M35">
      <xsl:apply-templates select="@*|node()" mode="M35"/>
   </xsl:template>

   <!--PATTERN hashstepath-->


	<!--RULE -->
<xsl:template match="ind-def:filehash_state/ind-def:path" priority="4000" mode="M36">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a filehash_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M36"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M36"/>
   <xsl:template match="@*|node()" priority="-2" mode="M36">
      <xsl:apply-templates select="@*|node()" mode="M36"/>
   </xsl:template>

   <!--PATTERN hashstefilename-->


	<!--RULE -->
<xsl:template match="ind-def:filehash_state/ind-def:filename" priority="4000" mode="M37">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a filehash_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M37"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M37"/>
   <xsl:template match="@*|node()" priority="-2" mode="M37">
      <xsl:apply-templates select="@*|node()" mode="M37"/>
   </xsl:template>

   <!--PATTERN hashstemd5-->


	<!--RULE -->
<xsl:template match="ind-def:filehash_state/ind-def:md5" priority="4000" mode="M38">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the md5 entity of a filehash_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M38"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M38"/>
   <xsl:template match="@*|node()" priority="-2" mode="M38">
      <xsl:apply-templates select="@*|node()" mode="M38"/>
   </xsl:template>

   <!--PATTERN hashstesha1-->


	<!--RULE -->
<xsl:template match="ind-def:filehash_state/ind-def:sha1" priority="4000" mode="M39">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sha1 entity of a filehash_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M39"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M39"/>
   <xsl:template match="@*|node()" priority="-2" mode="M39">
      <xsl:apply-templates select="@*|node()" mode="M39"/>
   </xsl:template>

   <!--PATTERN envtst-->


	<!--RULE -->
<xsl:template match="ind-def:environmentvariable_test/ind-def:object" priority="4000"
                 mode="M40">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:environmentvariable_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an environmentvariable_test must reference a environmentvariable_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M40"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:environmentvariable_test/ind-def:state" priority="3999"
                 mode="M40">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:environmentvariable_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an environmentvariable_test must reference a environmentvariable_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M40"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M40"/>
   <xsl:template match="@*|node()" priority="-2" mode="M40">
      <xsl:apply-templates select="@*|node()" mode="M40"/>
   </xsl:template>

   <!--PATTERN envobjname-->


	<!--RULE -->
<xsl:template match="ind-def:environmentvariable_object/ind-def:name" priority="4000"
                 mode="M41">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of an environmentvariable_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M41"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M41"/>
   <xsl:template match="@*|node()" priority="-2" mode="M41">
      <xsl:apply-templates select="@*|node()" mode="M41"/>
   </xsl:template>

   <!--PATTERN envstename-->


	<!--RULE -->
<xsl:template match="ind-def:environmentvariable_state/ind-def:name" priority="4000"
                 mode="M42">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of an environmentvariable_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M42"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M42"/>
   <xsl:template match="@*|node()" priority="-2" mode="M42">
      <xsl:apply-templates select="@*|node()" mode="M42"/>
   </xsl:template>

   <!--PATTERN envstevalue-->


	<!--RULE -->
<xsl:template match="ind-def:environmentvariable_state/ind-def:value" priority="4000"
                 mode="M43">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M43"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M43"/>
   <xsl:template match="@*|node()" priority="-2" mode="M43">
      <xsl:apply-templates select="@*|node()" mode="M43"/>
   </xsl:template>

   <!--PATTERN sqltst-->


	<!--RULE -->
<xsl:template match="ind-def:sql_test/ind-def:object" priority="4000" mode="M44">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:sql_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a sql_test must reference a sql_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M44"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:sql_test/ind-def:state" priority="3999" mode="M44">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:sql_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a sql_test must reference a sql_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M44"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M44"/>
   <xsl:template match="@*|node()" priority="-2" mode="M44">
      <xsl:apply-templates select="@*|node()" mode="M44"/>
   </xsl:template>

   <!--PATTERN sqlobjdengine-->


	<!--RULE -->
<xsl:template match="ind-def:sql_object/ind-def:engine" priority="4000" mode="M45">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the engine entity of an sql_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M45"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M45"/>
   <xsl:template match="@*|node()" priority="-2" mode="M45">
      <xsl:apply-templates select="@*|node()" mode="M45"/>
   </xsl:template>

   <!--PATTERN sqlobjversion-->


	<!--RULE -->
<xsl:template match="ind-def:sql_object/ind-def:version" priority="4000" mode="M46">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of an sql_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M46"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M46"/>
   <xsl:template match="@*|node()" priority="-2" mode="M46">
      <xsl:apply-templates select="@*|node()" mode="M46"/>
   </xsl:template>

   <!--PATTERN sqlobjconnection_string-->


	<!--RULE -->
<xsl:template match="ind-def:sql_object/ind-def:connection_string" priority="4000" mode="M47">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the connection_string entity of an sql_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M47"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M47"/>
   <xsl:template match="@*|node()" priority="-2" mode="M47">
      <xsl:apply-templates select="@*|node()" mode="M47"/>
   </xsl:template>

   <!--PATTERN sqlobjsql-->


	<!--RULE -->
<xsl:template match="ind-def:sql_object/ind-def:sql" priority="4000" mode="M48">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sql entity of a sql_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M48"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M48"/>
   <xsl:template match="@*|node()" priority="-2" mode="M48">
      <xsl:apply-templates select="@*|node()" mode="M48"/>
   </xsl:template>

   <!--PATTERN sqlsteengine-->


	<!--RULE -->
<xsl:template match="ind-def:sql_state/ind-def:engine" priority="4000" mode="M49">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the engine entity of an sql_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M49"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M49"/>
   <xsl:template match="@*|node()" priority="-2" mode="M49">
      <xsl:apply-templates select="@*|node()" mode="M49"/>
   </xsl:template>

   <!--PATTERN sqlsteversion-->


	<!--RULE -->
<xsl:template match="ind-def:sql_state/ind-def:version" priority="4000" mode="M50">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of an sql_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M50"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M50"/>
   <xsl:template match="@*|node()" priority="-2" mode="M50">
      <xsl:apply-templates select="@*|node()" mode="M50"/>
   </xsl:template>

   <!--PATTERN sqlsteconnection_string-->


	<!--RULE -->
<xsl:template match="ind-def:sql_state/ind-def:connection_string" priority="4000" mode="M51">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the connection_string entity of an sql_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M51"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M51"/>
   <xsl:template match="@*|node()" priority="-2" mode="M51">
      <xsl:apply-templates select="@*|node()" mode="M51"/>
   </xsl:template>

   <!--PATTERN sqlstesql-->


	<!--RULE -->
<xsl:template match="ind-def:sql_state/ind-def:sql" priority="4000" mode="M52">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sql entity of a sql_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M52"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M52"/>
   <xsl:template match="@*|node()" priority="-2" mode="M52">
      <xsl:apply-templates select="@*|node()" mode="M52"/>
   </xsl:template>

   <!--PATTERN sqlsteresult-->


	<!--RULE -->
<xsl:template match="ind-def:sql_state/ind-def:result" priority="4000" mode="M53">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M53"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M53"/>
   <xsl:template match="@*|node()" priority="-2" mode="M53">
      <xsl:apply-templates select="@*|node()" mode="M53"/>
   </xsl:template>

   <!--PATTERN txt54tst-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_test/ind-def:object" priority="4000"
                 mode="M54">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent54_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a textfilecontent54_test must reference a textfilecontent54_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M54"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:textfilecontent54_test/ind-def:state" priority="3999" mode="M54">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent54_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a textfilecontent54_test must reference a textfilecontent54_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M54"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M54"/>
   <xsl:template match="@*|node()" priority="-2" mode="M54">
      <xsl:apply-templates select="@*|node()" mode="M54"/>
   </xsl:template>

   <!--PATTERN txt54objpath-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_object/ind-def:path" priority="4000"
                 mode="M55">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a textfilecontent54_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M55"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M55"/>
   <xsl:template match="@*|node()" priority="-2" mode="M55">
      <xsl:apply-templates select="@*|node()" mode="M55"/>
   </xsl:template>

   <!--PATTERN txt54objfilename-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_object/ind-def:filename" priority="4000"
                 mode="M56">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a textfilecontent54_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M56"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M56"/>
   <xsl:template match="@*|node()" priority="-2" mode="M56">
      <xsl:apply-templates select="@*|node()" mode="M56"/>
   </xsl:template>

   <!--PATTERN txt54objpattern-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_object/ind-def:pattern" priority="4000"
                 mode="M57">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pattern entity of a textfilecontent54_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M57"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M57"/>
   <xsl:template match="@*|node()" priority="-2" mode="M57">
      <xsl:apply-templates select="@*|node()" mode="M57"/>
   </xsl:template>

   <!--PATTERN txt54objinstance-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_object/ind-def:instance" priority="4000"
                 mode="M58">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the instance entity of a textfilecontent54_object should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M58"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M58"/>
   <xsl:template match="@*|node()" priority="-2" mode="M58">
      <xsl:apply-templates select="@*|node()" mode="M58"/>
   </xsl:template>

   <!--PATTERN txt54stepath-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_state/ind-def:path" priority="4000" mode="M59">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a textfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M59"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M59"/>
   <xsl:template match="@*|node()" priority="-2" mode="M59">
      <xsl:apply-templates select="@*|node()" mode="M59"/>
   </xsl:template>

   <!--PATTERN txt54stefilename-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_state/ind-def:filename" priority="4000"
                 mode="M60">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a textfilecontent54_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M60"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M60"/>
   <xsl:template match="@*|node()" priority="-2" mode="M60">
      <xsl:apply-templates select="@*|node()" mode="M60"/>
   </xsl:template>

   <!--PATTERN txt54stepattern-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_state/ind-def:pattern" priority="4000"
                 mode="M61">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pattern entity of a textfilecontent54_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M61"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M61"/>
   <xsl:template match="@*|node()" priority="-2" mode="M61">
      <xsl:apply-templates select="@*|node()" mode="M61"/>
   </xsl:template>

   <!--PATTERN txt54steinstance-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_state/ind-def:instance" priority="4000"
                 mode="M62">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the instance entity of a textfilecontent54_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M62"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M62"/>
   <xsl:template match="@*|node()" priority="-2" mode="M62">
      <xsl:apply-templates select="@*|node()" mode="M62"/>
   </xsl:template>

   <!--PATTERN txt54stesubexpression-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent54_state/ind-def:subexpression" priority="4000"
                 mode="M63">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M63"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M63"/>
   <xsl:template match="@*|node()" priority="-2" mode="M63">
      <xsl:apply-templates select="@*|node()" mode="M63"/>
   </xsl:template>

   <!--PATTERN txttst-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_test/ind-def:object" priority="4000" mode="M64">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:textfilecontent_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a textfilecontent_test must reference a textfilecontent_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M64"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:textfilecontent_test/ind-def:state" priority="3999" mode="M64">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:textfilecontent_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a textfilecontent_test must reference a textfilecontent_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M64"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M64"/>
   <xsl:template match="@*|node()" priority="-2" mode="M64">
      <xsl:apply-templates select="@*|node()" mode="M64"/>
   </xsl:template>

   <!--PATTERN txtobjpath-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_object/ind-def:path" priority="4000" mode="M65">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a textfilecontent_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M65"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M65"/>
   <xsl:template match="@*|node()" priority="-2" mode="M65">
      <xsl:apply-templates select="@*|node()" mode="M65"/>
   </xsl:template>

   <!--PATTERN txtobjfilename-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_object/ind-def:filename" priority="4000"
                 mode="M66">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a textfilecontent_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M66"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M66"/>
   <xsl:template match="@*|node()" priority="-2" mode="M66">
      <xsl:apply-templates select="@*|node()" mode="M66"/>
   </xsl:template>

   <!--PATTERN txtobjline-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_object/ind-def:line" priority="4000" mode="M67">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the line entity of a textfilecontent_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M67"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M67"/>
   <xsl:template match="@*|node()" priority="-2" mode="M67">
      <xsl:apply-templates select="@*|node()" mode="M67"/>
   </xsl:template>

   <!--PATTERN txtstepath-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_state/ind-def:path" priority="4000" mode="M68">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a textfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M68"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M68"/>
   <xsl:template match="@*|node()" priority="-2" mode="M68">
      <xsl:apply-templates select="@*|node()" mode="M68"/>
   </xsl:template>

   <!--PATTERN txtstefilename-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_state/ind-def:filename" priority="4000"
                 mode="M69">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a textfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M69"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M69"/>
   <xsl:template match="@*|node()" priority="-2" mode="M69">
      <xsl:apply-templates select="@*|node()" mode="M69"/>
   </xsl:template>

   <!--PATTERN txtsteline-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_state/ind-def:line" priority="4000" mode="M70">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the line entity of a textfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M70"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M70"/>
   <xsl:template match="@*|node()" priority="-2" mode="M70">
      <xsl:apply-templates select="@*|node()" mode="M70"/>
   </xsl:template>

   <!--PATTERN txtstesubexpression-->


	<!--RULE -->
<xsl:template match="ind-def:textfilecontent_state/ind-def:subexpression" priority="4000"
                 mode="M71">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M71"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M71"/>
   <xsl:template match="@*|node()" priority="-2" mode="M71">
      <xsl:apply-templates select="@*|node()" mode="M71"/>
   </xsl:template>

   <!--PATTERN vattst-->


	<!--RULE -->
<xsl:template match="ind-def:variable_test/ind-def:object" priority="4000" mode="M72">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:variable_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a variable_test must reference a variable_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M72"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:variable_test/ind-def:state" priority="3999" mode="M72">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:variable_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a variable_test must reference a variable_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M72"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M72"/>
   <xsl:template match="@*|node()" priority="-2" mode="M72">
      <xsl:apply-templates select="@*|node()" mode="M72"/>
   </xsl:template>

   <!--PATTERN varobjvar_ref-->


	<!--RULE -->
<xsl:template match="ind-def:variable_object/ind-def:var_ref" priority="4000" mode="M73">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the var_ref entity of a variable_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M73"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M73"/>
   <xsl:template match="@*|node()" priority="-2" mode="M73">
      <xsl:apply-templates select="@*|node()" mode="M73"/>
   </xsl:template>

   <!--PATTERN varstevar_ref-->


	<!--RULE -->
<xsl:template match="ind-def:variable_state/ind-def:var_ref" priority="4000" mode="M74">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the var_ref entity of a variable_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M74"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M74"/>
   <xsl:template match="@*|node()" priority="-2" mode="M74">
      <xsl:apply-templates select="@*|node()" mode="M74"/>
   </xsl:template>

   <!--PATTERN varstevalue-->


	<!--RULE -->
<xsl:template match="ind-def:variable_state/ind-def:value" priority="4000" mode="M75">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M75"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M75"/>
   <xsl:template match="@*|node()" priority="-2" mode="M75">
      <xsl:apply-templates select="@*|node()" mode="M75"/>
   </xsl:template>

   <!--PATTERN xmltst-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_test/ind-def:object" priority="4000" mode="M76">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ind-def:xmlfilecontent_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a xmlfilecontent_test must reference a xmlfilecontent_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M76"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_test/ind-def:state" priority="3999" mode="M76">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ind-def:xmlfilecontent_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a xmlfilecontent_test must reference a xmlfilecontent_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M76"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M76"/>
   <xsl:template match="@*|node()" priority="-2" mode="M76">
      <xsl:apply-templates select="@*|node()" mode="M76"/>
   </xsl:template>

   <!--PATTERN xmlobjpath-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_object/ind-def:path" priority="4000" mode="M77">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a xmlfilecontent_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M77"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M77"/>
   <xsl:template match="@*|node()" priority="-2" mode="M77">
      <xsl:apply-templates select="@*|node()" mode="M77"/>
   </xsl:template>

   <!--PATTERN xmlobjfilename-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_object/ind-def:filename" priority="4000"
                 mode="M78">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a xmlfilecontent_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M78"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M78"/>
   <xsl:template match="@*|node()" priority="-2" mode="M78">
      <xsl:apply-templates select="@*|node()" mode="M78"/>
   </xsl:template>

   <!--PATTERN xmlobjxpath-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_object/ind-def:xpath" priority="4000" mode="M79">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the xpath entity of a xmlfilecontent_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M79"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M79"/>
   <xsl:template match="@*|node()" priority="-2" mode="M79">
      <xsl:apply-templates select="@*|node()" mode="M79"/>
   </xsl:template>

   <!--PATTERN xmlstepath-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_state/ind-def:path" priority="4000" mode="M80">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a xmlfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M80"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M80"/>
   <xsl:template match="@*|node()" priority="-2" mode="M80">
      <xsl:apply-templates select="@*|node()" mode="M80"/>
   </xsl:template>

   <!--PATTERN xmlstefilename-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_state/ind-def:filename" priority="4000"
                 mode="M81">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a xmlfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M81"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M81"/>
   <xsl:template match="@*|node()" priority="-2" mode="M81">
      <xsl:apply-templates select="@*|node()" mode="M81"/>
   </xsl:template>

   <!--PATTERN xmlstexpath-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_state/ind-def:xpath" priority="4000" mode="M82">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the xpath entity of a xmlfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M82"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M82"/>
   <xsl:template match="@*|node()" priority="-2" mode="M82">
      <xsl:apply-templates select="@*|node()" mode="M82"/>
   </xsl:template>

   <!--PATTERN xmlstevalue_of-->


	<!--RULE -->
<xsl:template match="ind-def:xmlfilecontent_state/ind-def:value_of" priority="4000"
                 mode="M83">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the value_of entity of a xmlfilecontent_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M83"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M83"/>
   <xsl:template match="@*|node()" priority="-2" mode="M83">
      <xsl:apply-templates select="@*|node()" mode="M83"/>
   </xsl:template>

   <!--PATTERN filesetobjflstinst-->


	<!--RULE -->
<xsl:template match="aix-def:fileset_object/aix-def:flstinst" priority="4000" mode="M84">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the flstinst entity of a fileset_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M84"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M84"/>
   <xsl:template match="@*|node()" priority="-2" mode="M84">
      <xsl:apply-templates select="@*|node()" mode="M84"/>
   </xsl:template>

   <!--PATTERN filesetsteflstinst-->


	<!--RULE -->
<xsl:template match="aix-def:fileset_state/aix-def:flstinst" priority="4000" mode="M85">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the flstinst entity of a fileset_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M85"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M85"/>
   <xsl:template match="@*|node()" priority="-2" mode="M85">
      <xsl:apply-templates select="@*|node()" mode="M85"/>
   </xsl:template>

   <!--PATTERN filesetstelevel-->


	<!--RULE -->
<xsl:template match="aix-def:fileset_state/aix-def:level" priority="4000" mode="M86">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the level entity of a fileset_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M86"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M86"/>
   <xsl:template match="@*|node()" priority="-2" mode="M86">
      <xsl:apply-templates select="@*|node()" mode="M86"/>
   </xsl:template>

   <!--PATTERN filesetstestate-->


	<!--RULE -->
<xsl:template match="aix-def:fileset_state/aix-def:state" priority="4000" mode="M87">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the state entity of a fileset_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M87"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M87"/>
   <xsl:template match="@*|node()" priority="-2" mode="M87">
      <xsl:apply-templates select="@*|node()" mode="M87"/>
   </xsl:template>

   <!--PATTERN filesetstedescription-->


	<!--RULE -->
<xsl:template match="aix-def:fileset_state/aix-def:description" priority="4000" mode="M88">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the description entity of a fileset_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M88"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M88"/>
   <xsl:template match="@*|node()" priority="-2" mode="M88">
      <xsl:apply-templates select="@*|node()" mode="M88"/>
   </xsl:template>

   <!--PATTERN fixobjapar_number-->


	<!--RULE -->
<xsl:template match="aix-def:fix_object/aix-def:apar_number" priority="4000" mode="M89">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the apar_number entity of a fix_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M89"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M89"/>
   <xsl:template match="@*|node()" priority="-2" mode="M89">
      <xsl:apply-templates select="@*|node()" mode="M89"/>
   </xsl:template>

   <!--PATTERN fixsteapar_number-->


	<!--RULE -->
<xsl:template match="aix-def:fix_state/aix-def:apar_number" priority="4000" mode="M90">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the apar_number entity of a fix_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M90"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M90"/>
   <xsl:template match="@*|node()" priority="-2" mode="M90">
      <xsl:apply-templates select="@*|node()" mode="M90"/>
   </xsl:template>

   <!--PATTERN fixsteabstract-->


	<!--RULE -->
<xsl:template match="aix-def:fix_state/aix-def:abstract" priority="4000" mode="M91">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the abstract entity of a fix_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M91"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M91"/>
   <xsl:template match="@*|node()" priority="-2" mode="M91">
      <xsl:apply-templates select="@*|node()" mode="M91"/>
   </xsl:template>

   <!--PATTERN fixstesymptom-->


	<!--RULE -->
<xsl:template match="aix-def:fix_state/aix-def:symptom" priority="4000" mode="M92">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the symptom entity of a fix_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M92"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M92"/>
   <xsl:template match="@*|node()" priority="-2" mode="M92">
      <xsl:apply-templates select="@*|node()" mode="M92"/>
   </xsl:template>

   <!--PATTERN fixsteinstallation_status-->


	<!--RULE -->
<xsl:template match="aix-def:fix_state/aix-def:installation_status" priority="4000"
                 mode="M93">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the installation_status entity of a fix_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M93"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M93"/>
   <xsl:template match="@*|node()" priority="-2" mode="M93">
      <xsl:apply-templates select="@*|node()" mode="M93"/>
   </xsl:template>

   <!--PATTERN oslevelstemaintenance_level-->


	<!--RULE -->
<xsl:template match="aix-def:oslevel_state/aix-def:maintenance_level" priority="4000"
                 mode="M94">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the maintenance_level entity of an oslevel_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M94"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M94"/>
   <xsl:template match="@*|node()" priority="-2" mode="M94">
      <xsl:apply-templates select="@*|node()" mode="M94"/>
   </xsl:template>

   <!--PATTERN httpdtst-->


	<!--RULE -->
<xsl:template match="apache-def:httpd_test/apache-def:object" priority="4000" mode="M95">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/apache-def:httpd_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a httpd_test must reference a httpd_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M95"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="apache-def:httpd_test/apache-def:state" priority="3999" mode="M95">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/apache-def:httpd_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a httpd_test must reference a httpd_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M95"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M95"/>
   <xsl:template match="@*|node()" priority="-2" mode="M95">
      <xsl:apply-templates select="@*|node()" mode="M95"/>
   </xsl:template>

   <!--PATTERN httpdstepath-->


	<!--RULE -->
<xsl:template match="apache-def:httpd_state/apache-def:path" priority="4000" mode="M96">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a httpd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M96"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M96"/>
   <xsl:template match="@*|node()" priority="-2" mode="M96">
      <xsl:apply-templates select="@*|node()" mode="M96"/>
   </xsl:template>

   <!--PATTERN httpdstebinaryname-->


	<!--RULE -->
<xsl:template match="apache-def:httpd_state/apache-def:binary_name" priority="4000"
                 mode="M97">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the binary_name entity of a httpd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M97"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M97"/>
   <xsl:template match="@*|node()" priority="-2" mode="M97">
      <xsl:apply-templates select="@*|node()" mode="M97"/>
   </xsl:template>

   <!--PATTERN httpdsteversion-->


	<!--RULE -->
<xsl:template match="apache-def:httpd_state/apache-def:version" priority="4000" mode="M98">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a httpd_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M98"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M98"/>
   <xsl:template match="@*|node()" priority="-2" mode="M98">
      <xsl:apply-templates select="@*|node()" mode="M98"/>
   </xsl:template>

   <!--PATTERN apachevertst-->


	<!--RULE -->
<xsl:template match="apache-def:version_test/apache-def:object" priority="4000" mode="M99">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/apache-def:version_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a version_test must reference a version_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M99"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="apache-def:version_test/apache-def:state" priority="3999" mode="M99">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/apache-def:version_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a version_test must reference a version_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M99"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M99"/>
   <xsl:template match="@*|node()" priority="-2" mode="M99">
      <xsl:apply-templates select="@*|node()" mode="M99"/>
   </xsl:template>

   <!--PATTERN apcheversteversion-->


	<!--RULE -->
<xsl:template match="apache-def:version_state/apache-def:version" priority="4000" mode="M100">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of an version_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M100"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M100"/>
   <xsl:template match="@*|node()" priority="-2" mode="M100">
      <xsl:apply-templates select="@*|node()" mode="M100"/>
   </xsl:template>

   <!--PATTERN catosmoduletst-->


	<!--RULE -->
<xsl:template match="catos-def:module_test/catos-def:object" priority="4000" mode="M101">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/catos-def:module_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a module_test must reference a module_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M101"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="catos-def:module_test/catos-def:state" priority="3999" mode="M101">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/catos-def:module_number/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a module_test must reference a module_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M101"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M101"/>
   <xsl:template match="@*|node()" priority="-2" mode="M101">
      <xsl:apply-templates select="@*|node()" mode="M101"/>
   </xsl:template>

   <!--PATTERN moduleobjmodule_number-->


	<!--RULE -->
<xsl:template match="catos-def:module_object/catos-def:module_number" priority="4000"
                 mode="M102">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the module_number entity of a module_object should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the module_number entity of a module_object should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', or 'less than or equal'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M102"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M102"/>
   <xsl:template match="@*|node()" priority="-2" mode="M102">
      <xsl:apply-templates select="@*|node()" mode="M102"/>
   </xsl:template>

   <!--PATTERN catosmodulestemodule_number-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:module_number" priority="4000"
                 mode="M103">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the module_number entity of a module_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M103"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M103"/>
   <xsl:template match="@*|node()" priority="-2" mode="M103">
      <xsl:apply-templates select="@*|node()" mode="M103"/>
   </xsl:template>

   <!--PATTERN catosmodulestetype-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:type" priority="4000" mode="M104">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the type entity of a module_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M104"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M104"/>
   <xsl:template match="@*|node()" priority="-2" mode="M104">
      <xsl:apply-templates select="@*|node()" mode="M104"/>
   </xsl:template>

   <!--PATTERN catosmodulestemodel-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:model" priority="4000" mode="M105">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the model entity of a module_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M105"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M105"/>
   <xsl:template match="@*|node()" priority="-2" mode="M105">
      <xsl:apply-templates select="@*|node()" mode="M105"/>
   </xsl:template>

   <!--PATTERN catosmodulestesoftware_major_release-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:software_major_release" priority="4000"
                 mode="M106">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the software_major_release entity of a module_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M106"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M106"/>
   <xsl:template match="@*|node()" priority="-2" mode="M106">
      <xsl:apply-templates select="@*|node()" mode="M106"/>
   </xsl:template>

   <!--PATTERN catosmodulestesoftware_individual_release-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:software_individual_release"
                 priority="4000"
                 mode="M107">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the software_individual_release entity of a module_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M107"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M107"/>
   <xsl:template match="@*|node()" priority="-2" mode="M107">
      <xsl:apply-templates select="@*|node()" mode="M107"/>
   </xsl:template>

   <!--PATTERN catosmodulestesoftware_version_id-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:software_version_id" priority="4000"
                 mode="M108">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the software_version_id entity of a module_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M108"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M108"/>
   <xsl:template match="@*|node()" priority="-2" mode="M108">
      <xsl:apply-templates select="@*|node()" mode="M108"/>
   </xsl:template>

   <!--PATTERN catosmodulestehardware_major_release-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:hardware_major_release" priority="4000"
                 mode="M109">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hardware_major_release entity of a module_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M109"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M109"/>
   <xsl:template match="@*|node()" priority="-2" mode="M109">
      <xsl:apply-templates select="@*|node()" mode="M109"/>
   </xsl:template>

   <!--PATTERN catosmodulestehardware_individual_release-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:hardware_individual_release"
                 priority="4000"
                 mode="M110">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hardware_individual_release entity of a module_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M110"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M110"/>
   <xsl:template match="@*|node()" priority="-2" mode="M110">
      <xsl:apply-templates select="@*|node()" mode="M110"/>
   </xsl:template>

   <!--PATTERN catosmodulestefirmware_major_release-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:firmware_major_release" priority="4000"
                 mode="M111">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the firmware_major_release entity of a module_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M111"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M111"/>
   <xsl:template match="@*|node()" priority="-2" mode="M111">
      <xsl:apply-templates select="@*|node()" mode="M111"/>
   </xsl:template>

   <!--PATTERN catosmodulestefirmware_individual_release-->


	<!--RULE -->
<xsl:template match="catos-def:module_state/catos-def:firmware_individual_release"
                 priority="4000"
                 mode="M112">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the firmware_individual_release entity of a module_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M112"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M112"/>
   <xsl:template match="@*|node()" priority="-2" mode="M112">
      <xsl:apply-templates select="@*|node()" mode="M112"/>
   </xsl:template>

   <!--PATTERN catosverstedevice_series-->


	<!--RULE -->
<xsl:template match="catos-def:version_state/catos-def:swtich_series" priority="4000"
                 mode="M113">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the swtich_series entity of a version_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M113"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M113"/>
   <xsl:template match="@*|node()" priority="-2" mode="M113">
      <xsl:apply-templates select="@*|node()" mode="M113"/>
   </xsl:template>

   <!--PATTERN catosversteimage_name-->


	<!--RULE -->
<xsl:template match="catos-def:version_state/catos-def:image_name" priority="4000"
                 mode="M114">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the image_name entity of a version_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M114"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M114"/>
   <xsl:template match="@*|node()" priority="-2" mode="M114">
      <xsl:apply-templates select="@*|node()" mode="M114"/>
   </xsl:template>

   <!--PATTERN catosverstecatos_major_release-->


	<!--RULE -->
<xsl:template match="catos-def:version_state/catos-def:catos_major_release" priority="4000"
                 mode="M115">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the catos_major_release entity of a version_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M115"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M115"/>
   <xsl:template match="@*|node()" priority="-2" mode="M115">
      <xsl:apply-templates select="@*|node()" mode="M115"/>
   </xsl:template>

   <!--PATTERN catosverstecatos_individual_release-->


	<!--RULE -->
<xsl:template match="catos-def:version_state/catos-def:catos_individual_release"
                 priority="4000"
                 mode="M116">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the catos_individual_release entity of a version_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M116"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M116"/>
   <xsl:template match="@*|node()" priority="-2" mode="M116">
      <xsl:apply-templates select="@*|node()" mode="M116"/>
   </xsl:template>

   <!--PATTERN catosverstecatos_version_id-->


	<!--RULE -->
<xsl:template match="catos-def:version_state/catos-def:catos_version_id" priority="4000"
                 mode="M117">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the catos_version_id entity of a version_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M117"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M117"/>
   <xsl:template match="@*|node()" priority="-2" mode="M117">
      <xsl:apply-templates select="@*|node()" mode="M117"/>
   </xsl:template>

   <!--PATTERN esxpatchobjpatch_number-->


	<!--RULE -->
<xsl:template match="esx-def:patch_object/esx-def:patch_number" priority="4000" mode="M118">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the patch_number entity of a patch_object should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M118"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M118"/>
   <xsl:template match="@*|node()" priority="-2" mode="M118">
      <xsl:apply-templates select="@*|node()" mode="M118"/>
   </xsl:template>

   <!--PATTERN esxpatchstepatch_number-->


	<!--RULE -->
<xsl:template match="esx-def:patch_state/esx-def:patch_number" priority="4000" mode="M119">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the patch_number entity of a patch_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M119"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M119"/>
   <xsl:template match="@*|node()" priority="-2" mode="M119">
      <xsl:apply-templates select="@*|node()" mode="M119"/>
   </xsl:template>

   <!--PATTERN esxpatchstestatus-->


	<!--RULE -->
<xsl:template match="esx-def:patch_state/esx-def:status" priority="4000" mode="M120">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the status entity of a patch_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M120"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M120"/>
   <xsl:template match="@*|node()" priority="-2" mode="M120">
      <xsl:apply-templates select="@*|node()" mode="M120"/>
   </xsl:template>

   <!--PATTERN versionsterelease-->


	<!--RULE -->
<xsl:template match="esx-def:version_state/esx-def:release" priority="4000" mode="M121">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the release entity of an version_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M121"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M121"/>
   <xsl:template match="@*|node()" priority="-2" mode="M121">
      <xsl:apply-templates select="@*|node()" mode="M121"/>
   </xsl:template>

   <!--PATTERN versionstebuild-->


	<!--RULE -->
<xsl:template match="esx-def:version_state/esx-def:build" priority="4000" mode="M122">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the build entity of an version_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M122"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M122"/>
   <xsl:template match="@*|node()" priority="-2" mode="M122">
      <xsl:apply-templates select="@*|node()" mode="M122"/>
   </xsl:template>

   <!--PATTERN portinfotst-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_test/freebsd-def:object" priority="4000"
                 mode="M123">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/freebsd-def:portinfo_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a portinfo_test must reference an portinfo_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M123"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="freebsd-def:portinfo_test/freebsd-def:state" priority="3999" mode="M123">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/freebsd-def:portinfo_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a portinfo_test must reference an portinfo_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M123"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M123"/>
   <xsl:template match="@*|node()" priority="-2" mode="M123">
      <xsl:apply-templates select="@*|node()" mode="M123"/>
   </xsl:template>

   <!--PATTERN portinfoobjpkginst-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_object/freebsd-def:pkginst" priority="4000"
                 mode="M124">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pkginst entity of a portinfo_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M124"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M124"/>
   <xsl:template match="@*|node()" priority="-2" mode="M124">
      <xsl:apply-templates select="@*|node()" mode="M124"/>
   </xsl:template>

   <!--PATTERN portinfostepkginst-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_state/freebsd-def:pkginst" priority="4000"
                 mode="M125">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pkginst entity of a portinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M125"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M125"/>
   <xsl:template match="@*|node()" priority="-2" mode="M125">
      <xsl:apply-templates select="@*|node()" mode="M125"/>
   </xsl:template>

   <!--PATTERN portinfostename-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_state/freebsd-def:name" priority="4000" mode="M126">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a portinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M126"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M126"/>
   <xsl:template match="@*|node()" priority="-2" mode="M126">
      <xsl:apply-templates select="@*|node()" mode="M126"/>
   </xsl:template>

   <!--PATTERN portinfostecategory-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_state/freebsd-def:category" priority="4000"
                 mode="M127">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the category entity of a portinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M127"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M127"/>
   <xsl:template match="@*|node()" priority="-2" mode="M127">
      <xsl:apply-templates select="@*|node()" mode="M127"/>
   </xsl:template>

   <!--PATTERN portinfosteversion-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_state/freebsd-def:version" priority="4000"
                 mode="M128">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a portinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M128"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M128"/>
   <xsl:template match="@*|node()" priority="-2" mode="M128">
      <xsl:apply-templates select="@*|node()" mode="M128"/>
   </xsl:template>

   <!--PATTERN portinfostevendor-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_state/freebsd-def:vendor" priority="4000"
                 mode="M129">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the vendor entity of a portinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M129"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M129"/>
   <xsl:template match="@*|node()" priority="-2" mode="M129">
      <xsl:apply-templates select="@*|node()" mode="M129"/>
   </xsl:template>

   <!--PATTERN portinfostedescription-->


	<!--RULE -->
<xsl:template match="freebsd-def:portinfo_state/freebsd-def:description" priority="4000"
                 mode="M130">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the description entity of a portinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M130"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M130"/>
   <xsl:template match="@*|node()" priority="-2" mode="M130">
      <xsl:apply-templates select="@*|node()" mode="M130"/>
   </xsl:template>

   <!--PATTERN gcobjparameter_name-->


	<!--RULE -->
<xsl:template match="hpux-def:getconf_object/hpux-def:parameter_name" priority="4000"
                 mode="M131">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the parameter_name entity of a getconf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M131"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M131"/>
   <xsl:template match="@*|node()" priority="-2" mode="M131">
      <xsl:apply-templates select="@*|node()" mode="M131"/>
   </xsl:template>

   <!--PATTERN gcobjpathname-->


	<!--RULE -->
<xsl:template match="hpux-def:getconf_object/hpux-def:pathname" priority="4000" mode="M132">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pathname entity of a getconf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M132"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M132"/>
   <xsl:template match="@*|node()" priority="-2" mode="M132">
      <xsl:apply-templates select="@*|node()" mode="M132"/>
   </xsl:template>

   <!--PATTERN gcsteparameter_name-->


	<!--RULE -->
<xsl:template match="hpux-def:getconf_state/hpux-def:parameter_name" priority="4000"
                 mode="M133">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the parameter_name entity of a getconf_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M133"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M133"/>
   <xsl:template match="@*|node()" priority="-2" mode="M133">
      <xsl:apply-templates select="@*|node()" mode="M133"/>
   </xsl:template>

   <!--PATTERN gcstepathname-->


	<!--RULE -->
<xsl:template match="hpux-def:getconf_state/hpux-def:pathname" priority="4000" mode="M134">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pathname entity of a getconf_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M134"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M134"/>
   <xsl:template match="@*|node()" priority="-2" mode="M134">
      <xsl:apply-templates select="@*|node()" mode="M134"/>
   </xsl:template>

   <!--PATTERN gcsteoutput-->


	<!--RULE -->
<xsl:template match="hpux-def:getconf_state/hpux-def:output" priority="4000" mode="M135">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the output entity of a getconf_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M135"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M135"/>
   <xsl:template match="@*|node()" priority="-2" mode="M135">
      <xsl:apply-templates select="@*|node()" mode="M135"/>
   </xsl:template>

   <!--PATTERN patch53objswtype-->


	<!--RULE -->
<xsl:template match="hpux-def:patch53_object/hpux-def:swtype" priority="4000" mode="M136">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the swtype entity of a patch53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M136"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M136"/>
   <xsl:template match="@*|node()" priority="-2" mode="M136">
      <xsl:apply-templates select="@*|node()" mode="M136"/>
   </xsl:template>

   <!--PATTERN patch53objarea_patched-->


	<!--RULE -->
<xsl:template match="hpux-def:patch_object/hpux-def:area_patched" priority="4000" mode="M137">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the area_patched entity of a patch53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M137"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M137"/>
   <xsl:template match="@*|node()" priority="-2" mode="M137">
      <xsl:apply-templates select="@*|node()" mode="M137"/>
   </xsl:template>

   <!--PATTERN patch53objpatch_base-->


	<!--RULE -->
<xsl:template match="hpux-def:patch_object/hpux-def:patch_base" priority="4000" mode="M138">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the patch_base entity of a patch53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M138"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M138"/>
   <xsl:template match="@*|node()" priority="-2" mode="M138">
      <xsl:apply-templates select="@*|node()" mode="M138"/>
   </xsl:template>

   <!--PATTERN patch53steswtype-->


	<!--RULE -->
<xsl:template match="hpux-def:patch53_state/hpux-def:swtype" priority="4000" mode="M139">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the swtype entity of a patch53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M139"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M139"/>
   <xsl:template match="@*|node()" priority="-2" mode="M139">
      <xsl:apply-templates select="@*|node()" mode="M139"/>
   </xsl:template>

   <!--PATTERN patch53stearea_patched-->


	<!--RULE -->
<xsl:template match="hpux-def:patch53_state/hpux-def:area_patched" priority="4000"
                 mode="M140">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the area_patched entity of a patch53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M140"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M140"/>
   <xsl:template match="@*|node()" priority="-2" mode="M140">
      <xsl:apply-templates select="@*|node()" mode="M140"/>
   </xsl:template>

   <!--PATTERN patch53stepatch_base-->


	<!--RULE -->
<xsl:template match="hpux-def:patch53_state/hpux-def:patch_base" priority="4000" mode="M141">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the patch_base entity of a patch53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M141"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M141"/>
   <xsl:template match="@*|node()" priority="-2" mode="M141">
      <xsl:apply-templates select="@*|node()" mode="M141"/>
   </xsl:template>

   <!--PATTERN patchobjpatch_name-->


	<!--RULE -->
<xsl:template match="hpux-def:patch_object/hpux-def:patch_name" priority="4000" mode="M142">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the patch_name entity of a patch_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M142"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M142"/>
   <xsl:template match="@*|node()" priority="-2" mode="M142">
      <xsl:apply-templates select="@*|node()" mode="M142"/>
   </xsl:template>

   <!--PATTERN patchstepatch_name-->


	<!--RULE -->
<xsl:template match="hpux-def:patch_state/hpux-def:patch_name" priority="4000" mode="M143">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the patch_name entity of a patch_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M143"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M143"/>
   <xsl:template match="@*|node()" priority="-2" mode="M143">
      <xsl:apply-templates select="@*|node()" mode="M143"/>
   </xsl:template>

   <!--PATTERN patchsteswtype-->


	<!--RULE -->
<xsl:template match="hpux-def:patch_state/hpux-def:swtype" priority="4000" mode="M144">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the swtype entity of a patch_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M144"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M144"/>
   <xsl:template match="@*|node()" priority="-2" mode="M144">
      <xsl:apply-templates select="@*|node()" mode="M144"/>
   </xsl:template>

   <!--PATTERN patchstearea_patched-->


	<!--RULE -->
<xsl:template match="hpux-def:patch_state/hpux-def:area_patched" priority="4000" mode="M145">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the area_patched entity of a patch_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M145"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M145"/>
   <xsl:template match="@*|node()" priority="-2" mode="M145">
      <xsl:apply-templates select="@*|node()" mode="M145"/>
   </xsl:template>

   <!--PATTERN patchstepatch_base-->


	<!--RULE -->
<xsl:template match="hpux-def:patch_state/hpux-def:patch_base" priority="4000" mode="M146">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the patch_base entity of a patch_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M146"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M146"/>
   <xsl:template match="@*|node()" priority="-2" mode="M146">
      <xsl:apply-templates select="@*|node()" mode="M146"/>
   </xsl:template>

   <!--PATTERN swlistobjswlist-->


	<!--RULE -->
<xsl:template match="hpux-def:swlist_object/hpux-def:swlist" priority="4000" mode="M147">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the swlist entity of a swlist_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M147"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M147"/>
   <xsl:template match="@*|node()" priority="-2" mode="M147">
      <xsl:apply-templates select="@*|node()" mode="M147"/>
   </xsl:template>

   <!--PATTERN swliststeswlist-->


	<!--RULE -->
<xsl:template match="hpux-def:swlist_state/hpux-def:swlist" priority="4000" mode="M148">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the swlist entity of a swlist_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M148"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M148"/>
   <xsl:template match="@*|node()" priority="-2" mode="M148">
      <xsl:apply-templates select="@*|node()" mode="M148"/>
   </xsl:template>

   <!--PATTERN swliststebundle-->


	<!--RULE -->
<xsl:template match="hpux-def:swlist_state/hpux-def:bundle" priority="4000" mode="M149">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the bundle entity of a swlist_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M149"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M149"/>
   <xsl:template match="@*|node()" priority="-2" mode="M149">
      <xsl:apply-templates select="@*|node()" mode="M149"/>
   </xsl:template>

   <!--PATTERN swliststefileset-->


	<!--RULE -->
<xsl:template match="hpux-def:swlist_state/hpux-def:fileset" priority="4000" mode="M150">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the fileset entity of a swlist_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M150"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M150"/>
   <xsl:template match="@*|node()" priority="-2" mode="M150">
      <xsl:apply-templates select="@*|node()" mode="M150"/>
   </xsl:template>

   <!--PATTERN swliststeversion-->


	<!--RULE -->
<xsl:template match="hpux-def:swlist_state/hpux-def:version" priority="4000" mode="M151">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='fileset_revision'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a swlist_state should be 'fileset_revision'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M151"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M151"/>
   <xsl:template match="@*|node()" priority="-2" mode="M151">
      <xsl:apply-templates select="@*|node()" mode="M151"/>
   </xsl:template>

   <!--PATTERN swliststetitle-->


	<!--RULE -->
<xsl:template match="hpux-def:swlist_state/hpux-def:title" priority="4000" mode="M152">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the title entity of a swlist_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M152"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M152"/>
   <xsl:template match="@*|node()" priority="-2" mode="M152">
      <xsl:apply-templates select="@*|node()" mode="M152"/>
   </xsl:template>

   <!--PATTERN swliststevendor-->


	<!--RULE -->
<xsl:template match="hpux-def:swlist_state/hpux-def:vendor" priority="4000" mode="M153">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the vendor entity of a swlist_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M153"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M153"/>
   <xsl:template match="@*|node()" priority="-2" mode="M153">
      <xsl:apply-templates select="@*|node()" mode="M153"/>
   </xsl:template>

   <!--PATTERN trustobjusername-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_object/hpux-def:username" priority="4000" mode="M154">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a trusted_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M154"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M154"/>
   <xsl:template match="@*|node()" priority="-2" mode="M154">
      <xsl:apply-templates select="@*|node()" mode="M154"/>
   </xsl:template>

   <!--PATTERN truststeusername-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:username" priority="4000" mode="M155">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M155"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M155"/>
   <xsl:template match="@*|node()" priority="-2" mode="M155">
      <xsl:apply-templates select="@*|node()" mode="M155"/>
   </xsl:template>

   <!--PATTERN truststeuid-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:uid" priority="4000" mode="M156">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the uid entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M156"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M156"/>
   <xsl:template match="@*|node()" priority="-2" mode="M156">
      <xsl:apply-templates select="@*|node()" mode="M156"/>
   </xsl:template>

   <!--PATTERN truststepassword-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:password" priority="4000" mode="M157">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the password entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M157"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M157"/>
   <xsl:template match="@*|node()" priority="-2" mode="M157">
      <xsl:apply-templates select="@*|node()" mode="M157"/>
   </xsl:template>

   <!--PATTERN truststeaccount_owner-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:account_owner" priority="4000"
                 mode="M158">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the account_owner entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M158"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M158"/>
   <xsl:template match="@*|node()" priority="-2" mode="M158">
      <xsl:apply-templates select="@*|node()" mode="M158"/>
   </xsl:template>

   <!--PATTERN truststeboot_auth-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:boot_auth" priority="4000" mode="M159">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the boot_auth entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M159"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M159"/>
   <xsl:template match="@*|node()" priority="-2" mode="M159">
      <xsl:apply-templates select="@*|node()" mode="M159"/>
   </xsl:template>

   <!--PATTERN truststeaudit_id-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:audit_id" priority="4000" mode="M160">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the audit_id entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M160"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M160"/>
   <xsl:template match="@*|node()" priority="-2" mode="M160">
      <xsl:apply-templates select="@*|node()" mode="M160"/>
   </xsl:template>

   <!--PATTERN truststeaudit_flag-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:audit_flag" priority="4000" mode="M161">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the audit_flag entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M161"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M161"/>
   <xsl:template match="@*|node()" priority="-2" mode="M161">
      <xsl:apply-templates select="@*|node()" mode="M161"/>
   </xsl:template>

   <!--PATTERN truststepw_change_min-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_change_min" priority="4000"
                 mode="M162">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_change_min entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M162"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M162"/>
   <xsl:template match="@*|node()" priority="-2" mode="M162">
      <xsl:apply-templates select="@*|node()" mode="M162"/>
   </xsl:template>

   <!--PATTERN truststepw_max_size-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_max_size" priority="4000" mode="M163">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_max_size entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M163"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M163"/>
   <xsl:template match="@*|node()" priority="-2" mode="M163">
      <xsl:apply-templates select="@*|node()" mode="M163"/>
   </xsl:template>

   <!--PATTERN truststepw_expiration-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_expiration" priority="4000"
                 mode="M164">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_expiration entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M164"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M164"/>
   <xsl:template match="@*|node()" priority="-2" mode="M164">
      <xsl:apply-templates select="@*|node()" mode="M164"/>
   </xsl:template>

   <!--PATTERN truststepw_life-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_life" priority="4000" mode="M165">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_life entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M165"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M165"/>
   <xsl:template match="@*|node()" priority="-2" mode="M165">
      <xsl:apply-templates select="@*|node()" mode="M165"/>
   </xsl:template>

   <!--PATTERN truststepw_change_s-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_change_s" priority="4000" mode="M166">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_change_s entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M166"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M166"/>
   <xsl:template match="@*|node()" priority="-2" mode="M166">
      <xsl:apply-templates select="@*|node()" mode="M166"/>
   </xsl:template>

   <!--PATTERN truststepw_change_u-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_change_u" priority="4000" mode="M167">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_change_u entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M167"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M167"/>
   <xsl:template match="@*|node()" priority="-2" mode="M167">
      <xsl:apply-templates select="@*|node()" mode="M167"/>
   </xsl:template>

   <!--PATTERN truststeacct_expire-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:acct_expire" priority="4000" mode="M168">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the acct_expire entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M168"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M168"/>
   <xsl:template match="@*|node()" priority="-2" mode="M168">
      <xsl:apply-templates select="@*|node()" mode="M168"/>
   </xsl:template>

   <!--PATTERN truststemax_llogin-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:max_llogin" priority="4000" mode="M169">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the max_llogin entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M169"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M169"/>
   <xsl:template match="@*|node()" priority="-2" mode="M169">
      <xsl:apply-templates select="@*|node()" mode="M169"/>
   </xsl:template>

   <!--PATTERN truststeexp_warning-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:exp_warning" priority="4000" mode="M170">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the exp_warning entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M170"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M170"/>
   <xsl:template match="@*|node()" priority="-2" mode="M170">
      <xsl:apply-templates select="@*|node()" mode="M170"/>
   </xsl:template>

   <!--PATTERN truststeusr_chg_pw-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:usr_chg_pw" priority="4000" mode="M171">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the usr_chg_pw entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M171"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M171"/>
   <xsl:template match="@*|node()" priority="-2" mode="M171">
      <xsl:apply-templates select="@*|node()" mode="M171"/>
   </xsl:template>

   <!--PATTERN truststegen_pw-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:gen_pw" priority="4000" mode="M172">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the gen_pw entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M172"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M172"/>
   <xsl:template match="@*|node()" priority="-2" mode="M172">
      <xsl:apply-templates select="@*|node()" mode="M172"/>
   </xsl:template>

   <!--PATTERN truststepw_restrict-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_restrict" priority="4000" mode="M173">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_restrict entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M173"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M173"/>
   <xsl:template match="@*|node()" priority="-2" mode="M173">
      <xsl:apply-templates select="@*|node()" mode="M173"/>
   </xsl:template>

   <!--PATTERN truststepw_null-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_null" priority="4000" mode="M174">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_null entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M174"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M174"/>
   <xsl:template match="@*|node()" priority="-2" mode="M174">
      <xsl:apply-templates select="@*|node()" mode="M174"/>
   </xsl:template>

   <!--PATTERN truststepw_gen_char-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_gen_char" priority="4000" mode="M175">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_gen_char entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M175"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M175"/>
   <xsl:template match="@*|node()" priority="-2" mode="M175">
      <xsl:apply-templates select="@*|node()" mode="M175"/>
   </xsl:template>

   <!--PATTERN truststepw_gen_let-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_gen_let" priority="4000" mode="M176">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_gen_let entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M176"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M176"/>
   <xsl:template match="@*|node()" priority="-2" mode="M176">
      <xsl:apply-templates select="@*|node()" mode="M176"/>
   </xsl:template>

   <!--PATTERN truststelogin_time-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:login_time" priority="4000" mode="M177">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the login_time entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M177"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M177"/>
   <xsl:template match="@*|node()" priority="-2" mode="M177">
      <xsl:apply-templates select="@*|node()" mode="M177"/>
   </xsl:template>

   <!--PATTERN truststepw_changer-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:pw_changer" priority="4000" mode="M178">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pw_changer entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M178"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M178"/>
   <xsl:template match="@*|node()" priority="-2" mode="M178">
      <xsl:apply-templates select="@*|node()" mode="M178"/>
   </xsl:template>

   <!--PATTERN truststelogin_time_s-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:login_time_s" priority="4000"
                 mode="M179">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the login_time_s entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M179"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M179"/>
   <xsl:template match="@*|node()" priority="-2" mode="M179">
      <xsl:apply-templates select="@*|node()" mode="M179"/>
   </xsl:template>

   <!--PATTERN truststelogin_time_u-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:login_time_u" priority="4000"
                 mode="M180">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the login_time_u entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M180"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M180"/>
   <xsl:template match="@*|node()" priority="-2" mode="M180">
      <xsl:apply-templates select="@*|node()" mode="M180"/>
   </xsl:template>

   <!--PATTERN truststelogin_tty_s-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:login_tty_s" priority="4000" mode="M181">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the login_tty_s entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M181"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M181"/>
   <xsl:template match="@*|node()" priority="-2" mode="M181">
      <xsl:apply-templates select="@*|node()" mode="M181"/>
   </xsl:template>

   <!--PATTERN truststelogin_tty_u-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:login_tty_u" priority="4000" mode="M182">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the login_tty_u entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M182"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M182"/>
   <xsl:template match="@*|node()" priority="-2" mode="M182">
      <xsl:apply-templates select="@*|node()" mode="M182"/>
   </xsl:template>

   <!--PATTERN truststenum_u_logins-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:num_u_logins" priority="4000"
                 mode="M183">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the num_u_logins entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M183"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M183"/>
   <xsl:template match="@*|node()" priority="-2" mode="M183">
      <xsl:apply-templates select="@*|node()" mode="M183"/>
   </xsl:template>

   <!--PATTERN truststemax_u_logins-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:max_u_logins" priority="4000"
                 mode="M184">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the max_u_logins entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M184"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M184"/>
   <xsl:template match="@*|node()" priority="-2" mode="M184">
      <xsl:apply-templates select="@*|node()" mode="M184"/>
   </xsl:template>

   <!--PATTERN truststelock_flag-->


	<!--RULE -->
<xsl:template match="hpux-def:trusted_state/hpux-def:lock_flag" priority="4000" mode="M185">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the lock_flag entity of a trusted_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M185"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M185"/>
   <xsl:template match="@*|node()" priority="-2" mode="M185">
      <xsl:apply-templates select="@*|node()" mode="M185"/>
   </xsl:template>

   <!--PATTERN globaltst-->


	<!--RULE -->
<xsl:template match="ios-def:global_test/ios-def:object" priority="4000" mode="M186">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:global_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a global_test must reference a global_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M186"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ios-def:global_test/ios-def:state" priority="3999" mode="M186">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:global_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a global_test must reference a global_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M186"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M186"/>
   <xsl:template match="@*|node()" priority="-2" mode="M186">
      <xsl:apply-templates select="@*|node()" mode="M186"/>
   </xsl:template>

   <!--PATTERN globalobjglobal_command-->


	<!--RULE -->
<xsl:template match="ios-def:global_object/ios-def:global_command" priority="4000"
                 mode="M187">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the global_command entity of a global_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M187"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M187"/>
   <xsl:template match="@*|node()" priority="-2" mode="M187">
      <xsl:apply-templates select="@*|node()" mode="M187"/>
   </xsl:template>

   <!--PATTERN globalsteglobal_command-->


	<!--RULE -->
<xsl:template match="ios-def:global_state/ios-def:global_command" priority="4000" mode="M188">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the global_command entity of a global_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M188"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M188"/>
   <xsl:template match="@*|node()" priority="-2" mode="M188">
      <xsl:apply-templates select="@*|node()" mode="M188"/>
   </xsl:template>

   <!--PATTERN iosinterfacetst-->


	<!--RULE -->
<xsl:template match="ios-def:interface_test/ios-def:object" priority="4000" mode="M189">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:interface_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an interface_test must reference an interface_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M189"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ios-def:interface_test/ios-def:state" priority="3999" mode="M189">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:interface_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an interface_test must reference an interface_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M189"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M189"/>
   <xsl:template match="@*|node()" priority="-2" mode="M189">
      <xsl:apply-templates select="@*|node()" mode="M189"/>
   </xsl:template>

   <!--PATTERN iosinterfaceobjname-->


	<!--RULE -->
<xsl:template match="ios-def:interface_object/ios-def:name" priority="4000" mode="M190">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of an interface_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M190"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M190"/>
   <xsl:template match="@*|node()" priority="-2" mode="M190">
      <xsl:apply-templates select="@*|node()" mode="M190"/>
   </xsl:template>

   <!--PATTERN iosinterfacestename-->


	<!--RULE -->
<xsl:template match="ios-def:interface_state/ios-def:name" priority="4000" mode="M191">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M191"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M191"/>
   <xsl:template match="@*|node()" priority="-2" mode="M191">
      <xsl:apply-templates select="@*|node()" mode="M191"/>
   </xsl:template>

   <!--PATTERN iosinterfacesteip_directed_broadcast_command-->


	<!--RULE -->
<xsl:template match="ios-def:interface_state/ios-def:ip_directed_broadcast_command"
                 priority="4000"
                 mode="M192">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ip_directed_broadcast_command entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M192"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M192"/>
   <xsl:template match="@*|node()" priority="-2" mode="M192">
      <xsl:apply-templates select="@*|node()" mode="M192"/>
   </xsl:template>

   <!--PATTERN iosinterfacesteno_ip_directed_broadcast_command-->


	<!--RULE -->
<xsl:template match="ios-def:interface_state/ios-def:no_ip_directed_broadcast_command"
                 priority="4000"
                 mode="M193">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the no_ip_directed_broadcast_command entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M193"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M193"/>
   <xsl:template match="@*|node()" priority="-2" mode="M193">
      <xsl:apply-templates select="@*|node()" mode="M193"/>
   </xsl:template>

   <!--PATTERN iosinterfacesteproxy_arp_command-->


	<!--RULE -->
<xsl:template match="ios-def:interface_state/ios-def:proxy_arp_command" priority="4000"
                 mode="M194">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the proxy_arp_command entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M194"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M194"/>
   <xsl:template match="@*|node()" priority="-2" mode="M194">
      <xsl:apply-templates select="@*|node()" mode="M194"/>
   </xsl:template>

   <!--PATTERN iosinterfacesteshutdown_command-->


	<!--RULE -->
<xsl:template match="ios-def:interface_state/ios-def:shutdown_command" priority="4000"
                 mode="M195">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the shutdown_command entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M195"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M195"/>
   <xsl:template match="@*|node()" priority="-2" mode="M195">
      <xsl:apply-templates select="@*|node()" mode="M195"/>
   </xsl:template>

   <!--PATTERN linetst-->


	<!--RULE -->
<xsl:template match="ios-def:line_test/ios-def:object" priority="4000" mode="M196">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:line_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a line_test must reference a line_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M196"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ios-def:line_test/ios-def:state" priority="3999" mode="M196">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:line_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a line_test must reference a line_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M196"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M196"/>
   <xsl:template match="@*|node()" priority="-2" mode="M196">
      <xsl:apply-templates select="@*|node()" mode="M196"/>
   </xsl:template>

   <!--PATTERN lineobjshow_subcommand-->


	<!--RULE -->
<xsl:template match="ios-def:line_object/ios-def:show_subcommand" priority="4000" mode="M197">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the show_subcommand entity of a line_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M197"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M197"/>
   <xsl:template match="@*|node()" priority="-2" mode="M197">
      <xsl:apply-templates select="@*|node()" mode="M197"/>
   </xsl:template>

   <!--PATTERN linesteshow_subcommand-->


	<!--RULE -->
<xsl:template match="ios-def:line_state/ios-def:show_subcommand" priority="4000" mode="M198">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the show_subcommand entity of a line_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M198"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M198"/>
   <xsl:template match="@*|node()" priority="-2" mode="M198">
      <xsl:apply-templates select="@*|node()" mode="M198"/>
   </xsl:template>

   <!--PATTERN linesteconfig_line-->


	<!--RULE -->
<xsl:template match="ios-def:line_state/ios-def:config_line" priority="4000" mode="M199">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the config_line entity of a line_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M199"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M199"/>
   <xsl:template match="@*|node()" priority="-2" mode="M199">
      <xsl:apply-templates select="@*|node()" mode="M199"/>
   </xsl:template>

   <!--PATTERN snmptst-->


	<!--RULE -->
<xsl:template match="ios-def:snmp_test/ios-def:object" priority="4000" mode="M200">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:snmp_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a snmp_test must reference a snmp_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M200"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ios-def:snmp_test/ios-def:state" priority="3999" mode="M200">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:snmp_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a snmp_test must reference a snmp_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M200"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M200"/>
   <xsl:template match="@*|node()" priority="-2" mode="M200">
      <xsl:apply-templates select="@*|node()" mode="M200"/>
   </xsl:template>

   <!--PATTERN snmpsteaccess_list-->


	<!--RULE -->
<xsl:template match="ios-def:snmp_state/ios-def:access_list" priority="4000" mode="M201">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_list entity of a snmp_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M201"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M201"/>
   <xsl:template match="@*|node()" priority="-2" mode="M201">
      <xsl:apply-templates select="@*|node()" mode="M201"/>
   </xsl:template>

   <!--PATTERN snmpstecommunity_name-->


	<!--RULE -->
<xsl:template match="ios-def:snmp_state/ios-def:community_name" priority="4000" mode="M202">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the community_name entity of a snmp_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M202"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M202"/>
   <xsl:template match="@*|node()" priority="-2" mode="M202">
      <xsl:apply-templates select="@*|node()" mode="M202"/>
   </xsl:template>

   <!--PATTERN tclshtst-->


	<!--RULE -->
<xsl:template match="ios-def:tclsh_test/ios-def:object" priority="4000" mode="M203">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:tclsh_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a tclsh_test must reference a tclsh_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M203"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ios-def:tclsh_test/ios-def:state" priority="3999" mode="M203">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:tclsh_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a tclsh_test must reference a tclsh_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M203"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M203"/>
   <xsl:template match="@*|node()" priority="-2" mode="M203">
      <xsl:apply-templates select="@*|node()" mode="M203"/>
   </xsl:template>

   <!--PATTERN tclshsteavailable-->


	<!--RULE -->
<xsl:template match="ios-def:tclsh_state/ios-def:available" priority="4000" mode="M204">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the available entity of a tclsh_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M204"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M204"/>
   <xsl:template match="@*|node()" priority="-2" mode="M204">
      <xsl:apply-templates select="@*|node()" mode="M204"/>
   </xsl:template>

   <!--PATTERN iosvertst-->


	<!--RULE -->
<xsl:template match="ios-def:version_test/ios-def:object" priority="4000" mode="M205">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/ios-def:version_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a version_test must reference a version_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M205"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="ios-def:version_test/ios-def:state" priority="3999" mode="M205">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/ios-def:version_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a version_test must reference a version_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M205"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M205"/>
   <xsl:template match="@*|node()" priority="-2" mode="M205">
      <xsl:apply-templates select="@*|node()" mode="M205"/>
   </xsl:template>

   <!--PATTERN iosverstemajor_release-->


	<!--RULE -->
<xsl:template match="ios-def:version_state/ios-def:major_release" priority="4000" mode="M206">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the major_release entity of a version_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M206"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M206"/>
   <xsl:template match="@*|node()" priority="-2" mode="M206">
      <xsl:apply-templates select="@*|node()" mode="M206"/>
   </xsl:template>

   <!--PATTERN iosverstetrain_number-->


	<!--RULE -->
<xsl:template match="ios-def:version_state/ios-def:train_number" priority="4000" mode="M207">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the train_number entity of a version_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M207"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M207"/>
   <xsl:template match="@*|node()" priority="-2" mode="M207">
      <xsl:apply-templates select="@*|node()" mode="M207"/>
   </xsl:template>

   <!--PATTERN iosverstetrain_identifier-->


	<!--RULE -->
<xsl:template match="ios-def:version_state/ios-def:train_identifier" priority="4000"
                 mode="M208">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the train_identifier entity of a version_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M208"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M208"/>
   <xsl:template match="@*|node()" priority="-2" mode="M208">
      <xsl:apply-templates select="@*|node()" mode="M208"/>
   </xsl:template>

   <!--PATTERN iosversteversion_string-->


	<!--RULE -->
<xsl:template match="ios-def:version_state/ios-def:version_string" priority="4000"
                 mode="M209">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='ios_version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version_string entity of a version_state should be 'ios_version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M209"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M209"/>
   <xsl:template match="@*|node()" priority="-2" mode="M209">
      <xsl:apply-templates select="@*|node()" mode="M209"/>
   </xsl:template>

   <!--PATTERN dpkgobjname-->


	<!--RULE -->
<xsl:template match="linux-def:dpkginfo_object/linux-def:name" priority="4000" mode="M210">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a dpkginfo_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the name entity of a dpkginfo_object should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M210"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M210"/>
   <xsl:template match="@*|node()" priority="-2" mode="M210">
      <xsl:apply-templates select="@*|node()" mode="M210"/>
   </xsl:template>

   <!--PATTERN dpkgstename-->


	<!--RULE -->
<xsl:template match="linux-def:dpkginfo_state/linux-def:name" priority="4000" mode="M211">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a dpkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the name entity of a dpkginfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M211"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M211"/>
   <xsl:template match="@*|node()" priority="-2" mode="M211">
      <xsl:apply-templates select="@*|node()" mode="M211"/>
   </xsl:template>

   <!--PATTERN dpkgstearch-->


	<!--RULE -->
<xsl:template match="linux-def:dpkginfo_state/linux-def:arch" priority="4000" mode="M212">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the arch entity of a dpkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the arch entity of a dpkginfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M212"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M212"/>
   <xsl:template match="@*|node()" priority="-2" mode="M212">
      <xsl:apply-templates select="@*|node()" mode="M212"/>
   </xsl:template>

   <!--PATTERN dpkgsteepoch-->


	<!--RULE -->
<xsl:template match="linux-def:dpkginfo_state/linux-def:epoch" priority="4000" mode="M213">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the epoch entity of a dpkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the epoch entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M213"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M213"/>
   <xsl:template match="@*|node()" priority="-2" mode="M213">
      <xsl:apply-templates select="@*|node()" mode="M213"/>
   </xsl:template>

   <!--PATTERN dpkgsterelease-->


	<!--RULE -->
<xsl:template match="linux-def:dpkginfo_state/linux-def:release" priority="4000" mode="M214">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the release entity of a dpkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the release entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M214"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M214"/>
   <xsl:template match="@*|node()" priority="-2" mode="M214">
      <xsl:apply-templates select="@*|node()" mode="M214"/>
   </xsl:template>

   <!--PATTERN dpkgsteversion-->


	<!--RULE -->
<xsl:template match="linux-def:dpkginfo_state/linux-def:version" priority="4000" mode="M215">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a dpkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the version entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M215"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M215"/>
   <xsl:template match="@*|node()" priority="-2" mode="M215">
      <xsl:apply-templates select="@*|node()" mode="M215"/>
   </xsl:template>

   <!--PATTERN dpkgsteevr-->


	<!--RULE -->
<xsl:template match="linux-def:dpkginfo_state/linux-def:evr" priority="4000" mode="M216">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='evr_string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the evr entity of a dpkginfo_state should be 'evr_string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or 'pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the evr entity of a dpkginfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M216"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M216"/>
   <xsl:template match="@*|node()" priority="-2" mode="M216">
      <xsl:apply-templates select="@*|node()" mode="M216"/>
   </xsl:template>

   <!--PATTERN ilsobjprotocol-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_object/linux-def:protocol"
                 priority="4000"
                 mode="M217">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of an inetlisteningservers_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the protocol entity of an inetlisteningservers_object should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M217"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M217"/>
   <xsl:template match="@*|node()" priority="-2" mode="M217">
      <xsl:apply-templates select="@*|node()" mode="M217"/>
   </xsl:template>

   <!--PATTERN ilsobjlocal_address-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_object/linux-def:local_address"
                 priority="4000"
                 mode="M218">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_address entity of an inetlisteningservers_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the local_address entity of an inetlisteningservers_object should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M218"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M218"/>
   <xsl:template match="@*|node()" priority="-2" mode="M218">
      <xsl:apply-templates select="@*|node()" mode="M218"/>
   </xsl:template>

   <!--PATTERN ilsobjlocal_port-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_object/linux-def:local_port"
                 priority="4000"
                 mode="M219">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_port entity of an inetlisteningservers_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the local_port entity of an inetlisteningservers_object should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M219"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M219"/>
   <xsl:template match="@*|node()" priority="-2" mode="M219">
      <xsl:apply-templates select="@*|node()" mode="M219"/>
   </xsl:template>

   <!--PATTERN ilssteprotocol-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:protocol" priority="4000"
                 mode="M220">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the protocol entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M220"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M220"/>
   <xsl:template match="@*|node()" priority="-2" mode="M220">
      <xsl:apply-templates select="@*|node()" mode="M220"/>
   </xsl:template>

   <!--PATTERN ilsstelocal_address-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:local_address"
                 priority="4000"
                 mode="M221">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the local_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M221"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M221"/>
   <xsl:template match="@*|node()" priority="-2" mode="M221">
      <xsl:apply-templates select="@*|node()" mode="M221"/>
   </xsl:template>

   <!--PATTERN ilsstelocal_port-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:local_port"
                 priority="4000"
                 mode="M222">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_port entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the local_port entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M222"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M222"/>
   <xsl:template match="@*|node()" priority="-2" mode="M222">
      <xsl:apply-templates select="@*|node()" mode="M222"/>
   </xsl:template>

   <!--PATTERN ilsstelocal_full_address-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:local_full_address"
                 priority="4000"
                 mode="M223">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_full_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the local_full_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M223"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M223"/>
   <xsl:template match="@*|node()" priority="-2" mode="M223">
      <xsl:apply-templates select="@*|node()" mode="M223"/>
   </xsl:template>

   <!--PATTERN ilssteprogram_name-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:program_name"
                 priority="4000"
                 mode="M224">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the program_name entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the program_name entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M224"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M224"/>
   <xsl:template match="@*|node()" priority="-2" mode="M224">
      <xsl:apply-templates select="@*|node()" mode="M224"/>
   </xsl:template>

   <!--PATTERN ilssteforeign_address-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:foreign_address"
                 priority="4000"
                 mode="M225">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the foreign_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the foreign_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M225"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M225"/>
   <xsl:template match="@*|node()" priority="-2" mode="M225">
      <xsl:apply-templates select="@*|node()" mode="M225"/>
   </xsl:template>

   <!--PATTERN ilssteforeign_port-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:foreign_port"
                 priority="4000"
                 mode="M226">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the foreign_port entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the foreign_port entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M226"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M226"/>
   <xsl:template match="@*|node()" priority="-2" mode="M226">
      <xsl:apply-templates select="@*|node()" mode="M226"/>
   </xsl:template>

   <!--PATTERN ilssteforeign_full_address-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:foreign_full_address"
                 priority="4000"
                 mode="M227">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the foreign_full_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the foreign_full_address entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M227"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M227"/>
   <xsl:template match="@*|node()" priority="-2" mode="M227">
      <xsl:apply-templates select="@*|node()" mode="M227"/>
   </xsl:template>

   <!--PATTERN ilsstepid-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:pid" priority="4000"
                 mode="M228">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pid entity of an inetlisteningservers_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the pid entity of an inetlisteningservers_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', or 'less than or equal'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M228"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M228"/>
   <xsl:template match="@*|node()" priority="-2" mode="M228">
      <xsl:apply-templates select="@*|node()" mode="M228"/>
   </xsl:template>

   <!--PATTERN ilssteuser_id-->


	<!--RULE -->
<xsl:template match="linux-def:inetlisteningservers_state/linux-def:user_id" priority="4000"
                 mode="M229">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_id entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the user_id entity of an inetlisteningservers_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M229"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M229"/>
   <xsl:template match="@*|node()" priority="-2" mode="M229">
      <xsl:apply-templates select="@*|node()" mode="M229"/>
   </xsl:template>

   <!--PATTERN rpmobjname-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_object/linux-def:name" priority="4000" mode="M230">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a rpminfo_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the name entity of a rpminfo_object should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M230"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M230"/>
   <xsl:template match="@*|node()" priority="-2" mode="M230">
      <xsl:apply-templates select="@*|node()" mode="M230"/>
   </xsl:template>

   <!--PATTERN rpmstename-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_state/linux-def:name" priority="4000" mode="M231">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a rpminfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the name entity of a rpminfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M231"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M231"/>
   <xsl:template match="@*|node()" priority="-2" mode="M231">
      <xsl:apply-templates select="@*|node()" mode="M231"/>
   </xsl:template>

   <!--PATTERN rpmstearch-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_state/linux-def:arch" priority="4000" mode="M232">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the arch entity of a rpminfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the arch entity of a rpminfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M232"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M232"/>
   <xsl:template match="@*|node()" priority="-2" mode="M232">
      <xsl:apply-templates select="@*|node()" mode="M232"/>
   </xsl:template>

   <!--PATTERN rpmsteepoch-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_state/linux-def:epoch" priority="4000" mode="M233">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the epoch entity of a rpminfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the epoch entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M233"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M233"/>
   <xsl:template match="@*|node()" priority="-2" mode="M233">
      <xsl:apply-templates select="@*|node()" mode="M233"/>
   </xsl:template>

   <!--PATTERN rpmsterelease-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_state/linux-def:release" priority="4000" mode="M234">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string' or @datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the release entity of a rpminfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the release entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M234"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M234"/>
   <xsl:template match="@*|node()" priority="-2" mode="M234">
      <xsl:apply-templates select="@*|node()" mode="M234"/>
   </xsl:template>

   <!--PATTERN rpmsteversion-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_state/linux-def:version" priority="4000" mode="M235">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string' or @datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a rpminfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the version entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M235"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M235"/>
   <xsl:template match="@*|node()" priority="-2" mode="M235">
      <xsl:apply-templates select="@*|node()" mode="M235"/>
   </xsl:template>

   <!--PATTERN rpmsteevr-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_state/linux-def:evr" priority="4000" mode="M236">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='evr_string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the evr entity of a rpminfo_state should be 'evr_string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='greater than' or @operation='greater than or equal' or @operation='less than' or @operation='less than or equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the evr entity of a rpminfo_state should be 'equals', 'not equal', 'greater than', 'greater than or equal', 'less than', 'less than or equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M236"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M236"/>
   <xsl:template match="@*|node()" priority="-2" mode="M236">
      <xsl:apply-templates select="@*|node()" mode="M236"/>
   </xsl:template>

   <!--PATTERN rpmstesignaturekeyid-->


	<!--RULE -->
<xsl:template match="linux-def:rpminfo_state/linux-def:signature_keyid" priority="4000"
                 mode="M237">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the signature_keyid entity of a rpminfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the signature_keyid entity of a rpminfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M237"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M237"/>
   <xsl:template match="@*|node()" priority="-2" mode="M237">
      <xsl:apply-templates select="@*|node()" mode="M237"/>
   </xsl:template>

   <!--PATTERN spkginfoobjname-->


	<!--RULE -->
<xsl:template match="linux-def:slackwarepkginfo_object/linux-def:name" priority="4000"
                 mode="M238">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a slackwarepkginfo_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the name entity of a slackwarepkginfo_object should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M238"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M238"/>
   <xsl:template match="@*|node()" priority="-2" mode="M238">
      <xsl:apply-templates select="@*|node()" mode="M238"/>
   </xsl:template>

   <!--PATTERN spkginfostename-->


	<!--RULE -->
<xsl:template match="linux-def:slackwarepkginfo_state/linux-def:name" priority="4000"
                 mode="M239">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a slackwarepkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the name entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M239"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M239"/>
   <xsl:template match="@*|node()" priority="-2" mode="M239">
      <xsl:apply-templates select="@*|node()" mode="M239"/>
   </xsl:template>

   <!--PATTERN spkginfosteversion-->


	<!--RULE -->
<xsl:template match="linux-def:slackwarepkginfo_state/linux-def:version" priority="4000"
                 mode="M240">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a slackwarepkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the version entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M240"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M240"/>
   <xsl:template match="@*|node()" priority="-2" mode="M240">
      <xsl:apply-templates select="@*|node()" mode="M240"/>
   </xsl:template>

   <!--PATTERN spkginfostearch-->


	<!--RULE -->
<xsl:template match="linux-def:slackwarepkginfo_state/linux-def:architecture" priority="4000"
                 mode="M241">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the architecture entity of a slackwarepkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the architecture entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M241"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M241"/>
   <xsl:template match="@*|node()" priority="-2" mode="M241">
      <xsl:apply-templates select="@*|node()" mode="M241"/>
   </xsl:template>

   <!--PATTERN spkginfosterevision-->


	<!--RULE -->
<xsl:template match="linux-def:slackwarepkginfo_state/linux-def:revision" priority="4000"
                 mode="M242">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the revision entity of a slackwarepkginfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@operation) or @operation='equals' or @operation='not equal' or @operation='pattern match'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - operation attribute for the revision entity of a slackwarepkginfo_state should be 'equals', 'not equal', or 'pattern match'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M242"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M242"/>
   <xsl:template match="@*|node()" priority="-2" mode="M242">
      <xsl:apply-templates select="@*|node()" mode="M242"/>
   </xsl:template>

   <!--PATTERN accountobjusername-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_object/macos-def:username" priority="4000"
                 mode="M243">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of an accountinfo_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M243"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M243"/>
   <xsl:template match="@*|node()" priority="-2" mode="M243">
      <xsl:apply-templates select="@*|node()" mode="M243"/>
   </xsl:template>

   <!--PATTERN accountsteusername-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_state/macos-def:username" priority="4000"
                 mode="M244">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of an accountinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M244"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M244"/>
   <xsl:template match="@*|node()" priority="-2" mode="M244">
      <xsl:apply-templates select="@*|node()" mode="M244"/>
   </xsl:template>

   <!--PATTERN accountstepassword-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_state/macos-def:password" priority="4000"
                 mode="M245">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the password entity of an accountinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M245"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M245"/>
   <xsl:template match="@*|node()" priority="-2" mode="M245">
      <xsl:apply-templates select="@*|node()" mode="M245"/>
   </xsl:template>

   <!--PATTERN accountsteuid-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_state/macos-def:uid" priority="4000" mode="M246">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the uid entity of an accountinfo_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M246"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M246"/>
   <xsl:template match="@*|node()" priority="-2" mode="M246">
      <xsl:apply-templates select="@*|node()" mode="M246"/>
   </xsl:template>

   <!--PATTERN accountstegid-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_state/macos-def:gid" priority="4000" mode="M247">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the gid entity of an accountinfo_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M247"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M247"/>
   <xsl:template match="@*|node()" priority="-2" mode="M247">
      <xsl:apply-templates select="@*|node()" mode="M247"/>
   </xsl:template>

   <!--PATTERN accountsterealname-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_state/macos-def:realname" priority="4000"
                 mode="M248">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the realname entity of an accountinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M248"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M248"/>
   <xsl:template match="@*|node()" priority="-2" mode="M248">
      <xsl:apply-templates select="@*|node()" mode="M248"/>
   </xsl:template>

   <!--PATTERN accountstehome_dir-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_state/macos-def:home_dir" priority="4000"
                 mode="M249">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the home_dir entity of an accountinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M249"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M249"/>
   <xsl:template match="@*|node()" priority="-2" mode="M249">
      <xsl:apply-templates select="@*|node()" mode="M249"/>
   </xsl:template>

   <!--PATTERN accountstelogin_shell-->


	<!--RULE -->
<xsl:template match="macos-def:accountinfo_state/macos-def:login_shell" priority="4000"
                 mode="M250">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the login_shell entity of an accountinfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M250"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M250"/>
   <xsl:template match="@*|node()" priority="-2" mode="M250">
      <xsl:apply-templates select="@*|node()" mode="M250"/>
   </xsl:template>

   <!--PATTERN macosilsobjprogram_name-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_object/macos-def:program_name"
                 priority="4000"
                 mode="M251">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the program_name entity of an inetlisteningservers_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M251"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M251"/>
   <xsl:template match="@*|node()" priority="-2" mode="M251">
      <xsl:apply-templates select="@*|node()" mode="M251"/>
   </xsl:template>

   <!--PATTERN macosilssteprogram_name-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:program_name"
                 priority="4000"
                 mode="M252">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the program_name entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M252"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M252"/>
   <xsl:template match="@*|node()" priority="-2" mode="M252">
      <xsl:apply-templates select="@*|node()" mode="M252"/>
   </xsl:template>

   <!--PATTERN macosilsstelocal_address-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:local_address"
                 priority="4000"
                 mode="M253">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M253"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M253"/>
   <xsl:template match="@*|node()" priority="-2" mode="M253">
      <xsl:apply-templates select="@*|node()" mode="M253"/>
   </xsl:template>

   <!--PATTERN macosilsstelocal_full_address-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:local_full_address"
                 priority="4000"
                 mode="M254">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_full_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M254"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M254"/>
   <xsl:template match="@*|node()" priority="-2" mode="M254">
      <xsl:apply-templates select="@*|node()" mode="M254"/>
   </xsl:template>

   <!--PATTERN macosilsstelocal_port-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:local_port"
                 priority="4000"
                 mode="M255">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_port entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M255"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M255"/>
   <xsl:template match="@*|node()" priority="-2" mode="M255">
      <xsl:apply-templates select="@*|node()" mode="M255"/>
   </xsl:template>

   <!--PATTERN macosilssteforeign_address-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:foreign_address"
                 priority="4000"
                 mode="M256">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the foreign_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M256"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M256"/>
   <xsl:template match="@*|node()" priority="-2" mode="M256">
      <xsl:apply-templates select="@*|node()" mode="M256"/>
   </xsl:template>

   <!--PATTERN macosilssteforeign_full_address-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:foreign_full_address"
                 priority="4000"
                 mode="M257">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the foreign_full_address entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M257"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M257"/>
   <xsl:template match="@*|node()" priority="-2" mode="M257">
      <xsl:apply-templates select="@*|node()" mode="M257"/>
   </xsl:template>

   <!--PATTERN macosilssteforeign_port-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:foreign_port"
                 priority="4000"
                 mode="M258">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the foreign_port entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M258"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M258"/>
   <xsl:template match="@*|node()" priority="-2" mode="M258">
      <xsl:apply-templates select="@*|node()" mode="M258"/>
   </xsl:template>

   <!--PATTERN macosilsstepid-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:pid" priority="4000"
                 mode="M259">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pid entity of an inetlisteningservers_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M259"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M259"/>
   <xsl:template match="@*|node()" priority="-2" mode="M259">
      <xsl:apply-templates select="@*|node()" mode="M259"/>
   </xsl:template>

   <!--PATTERN macosilssteprotocol-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:protocol" priority="4000"
                 mode="M260">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M260"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M260"/>
   <xsl:template match="@*|node()" priority="-2" mode="M260">
      <xsl:apply-templates select="@*|node()" mode="M260"/>
   </xsl:template>

   <!--PATTERN macosilssteuser_id-->


	<!--RULE -->
<xsl:template match="macos-def:inetlisteningservers_state/macos-def:user_id" priority="4000"
                 mode="M261">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_id entity of an inetlisteningservers_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M261"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M261"/>
   <xsl:template match="@*|node()" priority="-2" mode="M261">
      <xsl:apply-templates select="@*|node()" mode="M261"/>
   </xsl:template>

   <!--PATTERN nvramobjnvram_var-->


	<!--RULE -->
<xsl:template match="macos-def:nvram_object/macos-def:nvram_var" priority="4000" mode="M262">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the nvram_var entity of a nvram_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M262"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M262"/>
   <xsl:template match="@*|node()" priority="-2" mode="M262">
      <xsl:apply-templates select="@*|node()" mode="M262"/>
   </xsl:template>

   <!--PATTERN nvramstenvram_var-->


	<!--RULE -->
<xsl:template match="macos-def:nvram_state/macos-def:nvram_var" priority="4000" mode="M263">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the nvram_var entity of a nvram_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M263"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M263"/>
   <xsl:template match="@*|node()" priority="-2" mode="M263">
      <xsl:apply-templates select="@*|node()" mode="M263"/>
   </xsl:template>

   <!--PATTERN nvramstenvram_value-->


	<!--RULE -->
<xsl:template match="macos-def:nvram_state/macos-def:nvram_value" priority="4000" mode="M264">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the nvram_value entity of a nvram_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M264"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M264"/>
   <xsl:template match="@*|node()" priority="-2" mode="M264">
      <xsl:apply-templates select="@*|node()" mode="M264"/>
   </xsl:template>

   <!--PATTERN pwpobjusername-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_object/macos-def:username" priority="4000"
                 mode="M265">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a pwpolicy_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M265"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M265"/>
   <xsl:template match="@*|node()" priority="-2" mode="M265">
      <xsl:apply-templates select="@*|node()" mode="M265"/>
   </xsl:template>

   <!--PATTERN pwpobjuserpass-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_object/macos-def:userpass" priority="4000"
                 mode="M266">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the userpass entity of a pwpolicy_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M266"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M266"/>
   <xsl:template match="@*|node()" priority="-2" mode="M266">
      <xsl:apply-templates select="@*|node()" mode="M266"/>
   </xsl:template>

   <!--PATTERN pwpobjdirectory_node-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_object/macos-def:directory_node" priority="4000"
                 mode="M267">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the directory_node entity of a pwpolicy_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M267"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M267"/>
   <xsl:template match="@*|node()" priority="-2" mode="M267">
      <xsl:apply-templates select="@*|node()" mode="M267"/>
   </xsl:template>

   <!--PATTERN pwpsteusername-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:username" priority="4000" mode="M268">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a pwpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M268"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M268"/>
   <xsl:template match="@*|node()" priority="-2" mode="M268">
      <xsl:apply-templates select="@*|node()" mode="M268"/>
   </xsl:template>

   <!--PATTERN pwpsteuserpass-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:userpass" priority="4000" mode="M269">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the userpass entity of a pwpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M269"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M269"/>
   <xsl:template match="@*|node()" priority="-2" mode="M269">
      <xsl:apply-templates select="@*|node()" mode="M269"/>
   </xsl:template>

   <!--PATTERN pwpstedirectory_node-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:directory_node" priority="4000"
                 mode="M270">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the directory_node entity of a pwpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M270"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M270"/>
   <xsl:template match="@*|node()" priority="-2" mode="M270">
      <xsl:apply-templates select="@*|node()" mode="M270"/>
   </xsl:template>

   <!--PATTERN pwpstemaxChars-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:maxChars" priority="4000" mode="M271">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the maxChars entity of a pwpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M271"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M271"/>
   <xsl:template match="@*|node()" priority="-2" mode="M271">
      <xsl:apply-templates select="@*|node()" mode="M271"/>
   </xsl:template>

   <!--PATTERN pwpstemaxFailedLoginAttempts-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:maxFailedLoginAttempts"
                 priority="4000"
                 mode="M272">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the maxFailedLoginAttempts entity of a pwpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M272"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M272"/>
   <xsl:template match="@*|node()" priority="-2" mode="M272">
      <xsl:apply-templates select="@*|node()" mode="M272"/>
   </xsl:template>

   <!--PATTERN pwpsteminChars-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:minChars" priority="4000" mode="M273">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the minChars entity of a pwpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M273"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M273"/>
   <xsl:template match="@*|node()" priority="-2" mode="M273">
      <xsl:apply-templates select="@*|node()" mode="M273"/>
   </xsl:template>

   <!--PATTERN pwpstepasswordCannotBeName-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:passwordCannotBeName" priority="4000"
                 mode="M274">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the passwordCannotBeName entity of a pwpolicy_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M274"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M274"/>
   <xsl:template match="@*|node()" priority="-2" mode="M274">
      <xsl:apply-templates select="@*|node()" mode="M274"/>
   </xsl:template>

   <!--PATTERN pwpsterequiresAlpha-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:requiresAlpha" priority="4000"
                 mode="M275">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the requiresAlpha entity of a pwpolicy_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M275"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M275"/>
   <xsl:template match="@*|node()" priority="-2" mode="M275">
      <xsl:apply-templates select="@*|node()" mode="M275"/>
   </xsl:template>

   <!--PATTERN pwpsterequiresNumeric-->


	<!--RULE -->
<xsl:template match="macos-def:pwpolicy_state/macos-def:requiresNumeric" priority="4000"
                 mode="M276">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the requiresNumeric entity of a pwpolicy_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M276"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M276"/>
   <xsl:template match="@*|node()" priority="-2" mode="M276">
      <xsl:apply-templates select="@*|node()" mode="M276"/>
   </xsl:template>

   <!--PATTERN isainfotst-->


	<!--RULE -->
<xsl:template match="sol-def:isainfo_test/sol-def:object" priority="4000" mode="M277">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:smf_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an isainfo_test must reference an isainfo_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M277"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="sol-def:isainfo_test/sol-def:state" priority="3999" mode="M277">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:smf_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an isainfo_test must reference an isainfo_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M277"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M277"/>
   <xsl:template match="@*|node()" priority="-2" mode="M277">
      <xsl:apply-templates select="@*|node()" mode="M277"/>
   </xsl:template>

   <!--PATTERN isastebits-->


	<!--RULE -->
<xsl:template match="sol-def:isainfo_state/sol-def:bits" priority="4000" mode="M278">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the bits entity of an isainfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M278"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M278"/>
   <xsl:template match="@*|node()" priority="-2" mode="M278">
      <xsl:apply-templates select="@*|node()" mode="M278"/>
   </xsl:template>

   <!--PATTERN isastekernel_isa-->


	<!--RULE -->
<xsl:template match="sol-def:isainfo_state/sol-def:kernel_isa" priority="4000" mode="M279">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the kernel_isa entity of an isainfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M279"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M279"/>
   <xsl:template match="@*|node()" priority="-2" mode="M279">
      <xsl:apply-templates select="@*|node()" mode="M279"/>
   </xsl:template>

   <!--PATTERN isasteapplication_isa-->


	<!--RULE -->
<xsl:template match="sol-def:isainfo_state/sol-def:application_isa" priority="4000"
                 mode="M280">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the application_isa entity of an isainfo_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M280"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M280"/>
   <xsl:template match="@*|node()" priority="-2" mode="M280">
      <xsl:apply-templates select="@*|node()" mode="M280"/>
   </xsl:template>

   <!--PATTERN packagetst-->


	<!--RULE -->
<xsl:template match="sol-def:package_test/sol-def:object" priority="4000" mode="M281">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:package_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a package_test must reference a package_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M281"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="sol-def:package_test/sol-def:state" priority="3999" mode="M281">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:package_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a package_test must reference a package_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M281"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M281"/>
   <xsl:template match="@*|node()" priority="-2" mode="M281">
      <xsl:apply-templates select="@*|node()" mode="M281"/>
   </xsl:template>

   <!--PATTERN packageobjpkginst-->


	<!--RULE -->
<xsl:template match="sol-def:package_object/sol-def:pkginst" priority="4000" mode="M282">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pkginst entity of a package_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M282"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M282"/>
   <xsl:template match="@*|node()" priority="-2" mode="M282">
      <xsl:apply-templates select="@*|node()" mode="M282"/>
   </xsl:template>

   <!--PATTERN packagestepkginst-->


	<!--RULE -->
<xsl:template match="sol-def:package_state/sol-def:pkginst" priority="4000" mode="M283">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pkginst entity of a package_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M283"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M283"/>
   <xsl:template match="@*|node()" priority="-2" mode="M283">
      <xsl:apply-templates select="@*|node()" mode="M283"/>
   </xsl:template>

   <!--PATTERN packagestename-->


	<!--RULE -->
<xsl:template match="sol-def:package_state/sol-def:name" priority="4000" mode="M284">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a package_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M284"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M284"/>
   <xsl:template match="@*|node()" priority="-2" mode="M284">
      <xsl:apply-templates select="@*|node()" mode="M284"/>
   </xsl:template>

   <!--PATTERN packagestecategory-->


	<!--RULE -->
<xsl:template match="sol-def:package_state/sol-def:category" priority="4000" mode="M285">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the category entity of a package_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M285"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M285"/>
   <xsl:template match="@*|node()" priority="-2" mode="M285">
      <xsl:apply-templates select="@*|node()" mode="M285"/>
   </xsl:template>

   <!--PATTERN packagesteversion-->


	<!--RULE -->
<xsl:template match="sol-def:package_state/sol-def:version" priority="4000" mode="M286">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a package_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M286"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M286"/>
   <xsl:template match="@*|node()" priority="-2" mode="M286">
      <xsl:apply-templates select="@*|node()" mode="M286"/>
   </xsl:template>

   <!--PATTERN packagestevendor-->


	<!--RULE -->
<xsl:template match="sol-def:package_state/sol-def:vendor" priority="4000" mode="M287">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the vendor entity of a package_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M287"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M287"/>
   <xsl:template match="@*|node()" priority="-2" mode="M287">
      <xsl:apply-templates select="@*|node()" mode="M287"/>
   </xsl:template>

   <!--PATTERN packagestedescription-->


	<!--RULE -->
<xsl:template match="sol-def:package_state/sol-def:description" priority="4000" mode="M288">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the description entity of a package_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M288"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M288"/>
   <xsl:template match="@*|node()" priority="-2" mode="M288">
      <xsl:apply-templates select="@*|node()" mode="M288"/>
   </xsl:template>

   <!--PATTERN patch54tst-->


	<!--RULE -->
<xsl:template match="sol-def:patch54_test/sol-def:object" priority="4000" mode="M289">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:patch54_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a patch54_test must reference a patch54_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M289"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="sol-def:patch54_test/sol-def:state" priority="3999" mode="M289">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:patch_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a patch54_test must reference a patch_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M289"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M289"/>
   <xsl:template match="@*|node()" priority="-2" mode="M289">
      <xsl:apply-templates select="@*|node()" mode="M289"/>
   </xsl:template>

   <!--PATTERN patchtst-->


	<!--RULE -->
<xsl:template match="sol-def:patch_test/sol-def:object" priority="4000" mode="M290">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:patch_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a patch_test must reference a patch_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M290"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="sol-def:patch_test/sol-def:state" priority="3999" mode="M290">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:patch_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a patch_test must reference a patch_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M290"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M290"/>
   <xsl:template match="@*|node()" priority="-2" mode="M290">
      <xsl:apply-templates select="@*|node()" mode="M290"/>
   </xsl:template>

   <!--PATTERN patch54objbase-->


	<!--RULE -->
<xsl:template match="sol-def:patch54_object/sol-def:base" priority="4000" mode="M291">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the base entity of a patch54_object should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M291"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M291"/>
   <xsl:template match="@*|node()" priority="-2" mode="M291">
      <xsl:apply-templates select="@*|node()" mode="M291"/>
   </xsl:template>

   <!--PATTERN patch54objversion-->


	<!--RULE -->
<xsl:template match="sol-def:patch54_object/sol-def:version" priority="4000" mode="M292">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a patch54_object should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M292"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M292"/>
   <xsl:template match="@*|node()" priority="-2" mode="M292">
      <xsl:apply-templates select="@*|node()" mode="M292"/>
   </xsl:template>

   <!--PATTERN patchobjbase-->


	<!--RULE -->
<xsl:template match="sol-def:patch_object/sol-def:base" priority="4000" mode="M293">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the base entity of a patch_object should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M293"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M293"/>
   <xsl:template match="@*|node()" priority="-2" mode="M293">
      <xsl:apply-templates select="@*|node()" mode="M293"/>
   </xsl:template>

   <!--PATTERN patchstebase-->


	<!--RULE -->
<xsl:template match="sol-def:patch_state/sol-def:base" priority="4000" mode="M294">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the base entity of a patch_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M294"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M294"/>
   <xsl:template match="@*|node()" priority="-2" mode="M294">
      <xsl:apply-templates select="@*|node()" mode="M294"/>
   </xsl:template>

   <!--PATTERN patchsteversion-->


	<!--RULE -->
<xsl:template match="sol-def:patch_state/sol-def:version" priority="4000" mode="M295">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a patch_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M295"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M295"/>
   <xsl:template match="@*|node()" priority="-2" mode="M295">
      <xsl:apply-templates select="@*|node()" mode="M295"/>
   </xsl:template>

   <!--PATTERN smftst-->


	<!--RULE -->
<xsl:template match="sol-def:smf_test/sol-def:object" priority="4000" mode="M296">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/sol-def:smf_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a smf_test must reference a smf_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M296"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="sol-def:smf_test/sol-def:state" priority="3999" mode="M296">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/sol-def:smf_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a smf_test must reference a smf_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M296"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M296"/>
   <xsl:template match="@*|node()" priority="-2" mode="M296">
      <xsl:apply-templates select="@*|node()" mode="M296"/>
   </xsl:template>

   <!--PATTERN fmriobjbase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_object/sol-def:fmri" priority="4000" mode="M297">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the fmri entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M297"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M297"/>
   <xsl:template match="@*|node()" priority="-2" mode="M297">
      <xsl:apply-templates select="@*|node()" mode="M297"/>
   </xsl:template>

   <!--PATTERN fmristebase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_state/sol-def:fmri" priority="4000" mode="M298">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the fmri entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M298"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M298"/>
   <xsl:template match="@*|node()" priority="-2" mode="M298">
      <xsl:apply-templates select="@*|node()" mode="M298"/>
   </xsl:template>

   <!--PATTERN srvnamestebase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_state/sol-def:service_name" priority="4000" mode="M299">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_name entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M299"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M299"/>
   <xsl:template match="@*|node()" priority="-2" mode="M299">
      <xsl:apply-templates select="@*|node()" mode="M299"/>
   </xsl:template>

   <!--PATTERN srvstatestebase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_state/sol-def:service_state" priority="4000" mode="M300">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_state entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M300"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M300"/>
   <xsl:template match="@*|node()" priority="-2" mode="M300">
      <xsl:apply-templates select="@*|node()" mode="M300"/>
   </xsl:template>

   <!--PATTERN protocolstebase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_state/sol-def:protocol" priority="4000" mode="M301">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M301"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M301"/>
   <xsl:template match="@*|node()" priority="-2" mode="M301">
      <xsl:apply-templates select="@*|node()" mode="M301"/>
   </xsl:template>

   <!--PATTERN srvexestebase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_state/sol-def:server_executable" priority="4000" mode="M302">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the server_executable entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M302"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M302"/>
   <xsl:template match="@*|node()" priority="-2" mode="M302">
      <xsl:apply-templates select="@*|node()" mode="M302"/>
   </xsl:template>

   <!--PATTERN srvargsstebase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_state/sol-def:server_arguements" priority="4000" mode="M303">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the server_arguements entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M303"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M303"/>
   <xsl:template match="@*|node()" priority="-2" mode="M303">
      <xsl:apply-templates select="@*|node()" mode="M303"/>
   </xsl:template>

   <!--PATTERN execasuserstebase-->


	<!--RULE -->
<xsl:template match="sol-def:smf_state/sol-def:exec_as_user" priority="4000" mode="M304">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the exec_as_user entity of a smf_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M304"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M304"/>
   <xsl:template match="@*|node()" priority="-2" mode="M304">
      <xsl:apply-templates select="@*|node()" mode="M304"/>
   </xsl:template>

   <!--PATTERN unixfileobjpath-->


	<!--RULE -->
<xsl:template match="unix-def:file_object/unix-def:path" priority="4000" mode="M305">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a file_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M305"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M305"/>
   <xsl:template match="@*|node()" priority="-2" mode="M305">
      <xsl:apply-templates select="@*|node()" mode="M305"/>
   </xsl:template>

   <!--PATTERN unixfileobjfilename-->


	<!--RULE -->
<xsl:template match="unix-def:file_object/unix-def:filename" priority="4000" mode="M306">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a file_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M306"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M306"/>
   <xsl:template match="@*|node()" priority="-2" mode="M306">
      <xsl:apply-templates select="@*|node()" mode="M306"/>
   </xsl:template>

   <!--PATTERN unixfilestepath-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:path" priority="4000" mode="M307">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M307"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M307"/>
   <xsl:template match="@*|node()" priority="-2" mode="M307">
      <xsl:apply-templates select="@*|node()" mode="M307"/>
   </xsl:template>

   <!--PATTERN unixfilestefilename-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:filename" priority="4000" mode="M308">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M308"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M308"/>
   <xsl:template match="@*|node()" priority="-2" mode="M308">
      <xsl:apply-templates select="@*|node()" mode="M308"/>
   </xsl:template>

   <!--PATTERN unixfilestetype-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:type" priority="4000" mode="M309">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the type entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M309"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M309"/>
   <xsl:template match="@*|node()" priority="-2" mode="M309">
      <xsl:apply-templates select="@*|node()" mode="M309"/>
   </xsl:template>

   <!--PATTERN unixfilestegroup_id-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:group_id" priority="4000" mode="M310">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group_id entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M310"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M310"/>
   <xsl:template match="@*|node()" priority="-2" mode="M310">
      <xsl:apply-templates select="@*|node()" mode="M310"/>
   </xsl:template>

   <!--PATTERN unixfilesteuser_id-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:user_id" priority="4000" mode="M311">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_id entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M311"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M311"/>
   <xsl:template match="@*|node()" priority="-2" mode="M311">
      <xsl:apply-templates select="@*|node()" mode="M311"/>
   </xsl:template>

   <!--PATTERN unixfilestea_time-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:a_time" priority="4000" mode="M312">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the a_time entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M312"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M312"/>
   <xsl:template match="@*|node()" priority="-2" mode="M312">
      <xsl:apply-templates select="@*|node()" mode="M312"/>
   </xsl:template>

   <!--PATTERN unixfilestec_time-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:c_time" priority="4000" mode="M313">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the c_time entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M313"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M313"/>
   <xsl:template match="@*|node()" priority="-2" mode="M313">
      <xsl:apply-templates select="@*|node()" mode="M313"/>
   </xsl:template>

   <!--PATTERN unixfilestem_time-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:m_time" priority="4000" mode="M314">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the m_time entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M314"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M314"/>
   <xsl:template match="@*|node()" priority="-2" mode="M314">
      <xsl:apply-templates select="@*|node()" mode="M314"/>
   </xsl:template>

   <!--PATTERN unixfilestesize-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:size" priority="4000" mode="M315">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the size entity of a file_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M315"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M315"/>
   <xsl:template match="@*|node()" priority="-2" mode="M315">
      <xsl:apply-templates select="@*|node()" mode="M315"/>
   </xsl:template>

   <!--PATTERN unixfilestesuid-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:suid" priority="4000" mode="M316">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the suid entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M316"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M316"/>
   <xsl:template match="@*|node()" priority="-2" mode="M316">
      <xsl:apply-templates select="@*|node()" mode="M316"/>
   </xsl:template>

   <!--PATTERN unixfilestesgid-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:sgid" priority="4000" mode="M317">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sgid entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M317"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M317"/>
   <xsl:template match="@*|node()" priority="-2" mode="M317">
      <xsl:apply-templates select="@*|node()" mode="M317"/>
   </xsl:template>

   <!--PATTERN unixfilestesticky-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:sticky" priority="4000" mode="M318">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sticky entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M318"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M318"/>
   <xsl:template match="@*|node()" priority="-2" mode="M318">
      <xsl:apply-templates select="@*|node()" mode="M318"/>
   </xsl:template>

   <!--PATTERN unixfilesteuread-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:uread" priority="4000" mode="M319">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the uread entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M319"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M319"/>
   <xsl:template match="@*|node()" priority="-2" mode="M319">
      <xsl:apply-templates select="@*|node()" mode="M319"/>
   </xsl:template>

   <!--PATTERN unixfilesteuwrite-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:uwrite" priority="4000" mode="M320">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the uwrite entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M320"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M320"/>
   <xsl:template match="@*|node()" priority="-2" mode="M320">
      <xsl:apply-templates select="@*|node()" mode="M320"/>
   </xsl:template>

   <!--PATTERN unixfilesteuexec-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:uexec" priority="4000" mode="M321">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the uexec entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M321"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M321"/>
   <xsl:template match="@*|node()" priority="-2" mode="M321">
      <xsl:apply-templates select="@*|node()" mode="M321"/>
   </xsl:template>

   <!--PATTERN unixfilestegread-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:gread" priority="4000" mode="M322">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the gread entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M322"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M322"/>
   <xsl:template match="@*|node()" priority="-2" mode="M322">
      <xsl:apply-templates select="@*|node()" mode="M322"/>
   </xsl:template>

   <!--PATTERN unixfilestegwrite-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:gwrite" priority="4000" mode="M323">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the gwrite entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M323"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M323"/>
   <xsl:template match="@*|node()" priority="-2" mode="M323">
      <xsl:apply-templates select="@*|node()" mode="M323"/>
   </xsl:template>

   <!--PATTERN unixfilestegexec-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:gexec" priority="4000" mode="M324">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the gexec entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M324"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M324"/>
   <xsl:template match="@*|node()" priority="-2" mode="M324">
      <xsl:apply-templates select="@*|node()" mode="M324"/>
   </xsl:template>

   <!--PATTERN unixfilesteoread-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:oread" priority="4000" mode="M325">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the oread entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M325"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M325"/>
   <xsl:template match="@*|node()" priority="-2" mode="M325">
      <xsl:apply-templates select="@*|node()" mode="M325"/>
   </xsl:template>

   <!--PATTERN unixfilesteowrite-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:owrite" priority="4000" mode="M326">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the owrite entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M326"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M326"/>
   <xsl:template match="@*|node()" priority="-2" mode="M326">
      <xsl:apply-templates select="@*|node()" mode="M326"/>
   </xsl:template>

   <!--PATTERN unixfilesteoexec-->


	<!--RULE -->
<xsl:template match="unix-def:file_state/unix-def:oexec" priority="4000" mode="M327">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the oexec entity of a file_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M327"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M327"/>
   <xsl:template match="@*|node()" priority="-2" mode="M327">
      <xsl:apply-templates select="@*|node()" mode="M327"/>
   </xsl:template>

   <!--PATTERN inetdobjprotocol-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_object/unix-def:protocol" priority="4000" mode="M328">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of an inetd_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M328"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M328"/>
   <xsl:template match="@*|node()" priority="-2" mode="M328">
      <xsl:apply-templates select="@*|node()" mode="M328"/>
   </xsl:template>

   <!--PATTERN inetdobjservice_name-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_object/unix-def:service_name" priority="4000" mode="M329">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_name entity of an inetd_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M329"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M329"/>
   <xsl:template match="@*|node()" priority="-2" mode="M329">
      <xsl:apply-templates select="@*|node()" mode="M329"/>
   </xsl:template>

   <!--PATTERN inetdsteprotocol-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_state/unix-def:protocol" priority="4000" mode="M330">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of an inetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M330"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M330"/>
   <xsl:template match="@*|node()" priority="-2" mode="M330">
      <xsl:apply-templates select="@*|node()" mode="M330"/>
   </xsl:template>

   <!--PATTERN inetdsteservice_name-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_state/unix-def:service_name" priority="4000" mode="M331">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_name entity of an inetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M331"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M331"/>
   <xsl:template match="@*|node()" priority="-2" mode="M331">
      <xsl:apply-templates select="@*|node()" mode="M331"/>
   </xsl:template>

   <!--PATTERN inetdsteserver_program-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_state/unix-def:server_program" priority="4000"
                 mode="M332">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the server_program entity of an inetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M332"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M332"/>
   <xsl:template match="@*|node()" priority="-2" mode="M332">
      <xsl:apply-templates select="@*|node()" mode="M332"/>
   </xsl:template>

   <!--PATTERN inetdsteserver_arguments-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_state/unix-def:server_arguments" priority="4000"
                 mode="M333">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the server_arguments entity of an inetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M333"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M333"/>
   <xsl:template match="@*|node()" priority="-2" mode="M333">
      <xsl:apply-templates select="@*|node()" mode="M333"/>
   </xsl:template>

   <!--PATTERN inetdsteendpoint_type-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_state/unix-def:endpoint_type" priority="4000" mode="M334">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the endpoint_type entity of an inetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M334"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M334"/>
   <xsl:template match="@*|node()" priority="-2" mode="M334">
      <xsl:apply-templates select="@*|node()" mode="M334"/>
   </xsl:template>

   <!--PATTERN inetdsteexec_as_user-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_state/unix-def:exec_as_user" priority="4000" mode="M335">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the exec_as_user entity of an inetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M335"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M335"/>
   <xsl:template match="@*|node()" priority="-2" mode="M335">
      <xsl:apply-templates select="@*|node()" mode="M335"/>
   </xsl:template>

   <!--PATTERN inetdstewait_status-->


	<!--RULE -->
<xsl:template match="unix-def:inetd_state/unix-def:wait_status" priority="4000" mode="M336">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the wait_status entity of an inetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M336"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M336"/>
   <xsl:template match="@*|node()" priority="-2" mode="M336">
      <xsl:apply-templates select="@*|node()" mode="M336"/>
   </xsl:template>

   <!--PATTERN unixinterfaceobjname-->


	<!--RULE -->
<xsl:template match="unix-def:interface_object/unix-def:name" priority="4000" mode="M337">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of an interface_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M337"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M337"/>
   <xsl:template match="@*|node()" priority="-2" mode="M337">
      <xsl:apply-templates select="@*|node()" mode="M337"/>
   </xsl:template>

   <!--PATTERN unixinterfacestename-->


	<!--RULE -->
<xsl:template match="unix-def:interface_state/unix-def:name" priority="4000" mode="M338">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M338"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M338"/>
   <xsl:template match="@*|node()" priority="-2" mode="M338">
      <xsl:apply-templates select="@*|node()" mode="M338"/>
   </xsl:template>

   <!--PATTERN unixinterfacestehardware_addr-->


	<!--RULE -->
<xsl:template match="unix-def:interface_state/unix-def:hardware_addr" priority="4000"
                 mode="M339">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hardware_addr entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M339"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M339"/>
   <xsl:template match="@*|node()" priority="-2" mode="M339">
      <xsl:apply-templates select="@*|node()" mode="M339"/>
   </xsl:template>

   <!--PATTERN unixinterfacesteinet_addr-->


	<!--RULE -->
<xsl:template match="unix-def:interface_state/unix-def:inet_addr" priority="4000" mode="M340">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the inet_addr entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M340"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M340"/>
   <xsl:template match="@*|node()" priority="-2" mode="M340">
      <xsl:apply-templates select="@*|node()" mode="M340"/>
   </xsl:template>

   <!--PATTERN unixinterfacestebroadcast_addr-->


	<!--RULE -->
<xsl:template match="unix-def:interface_state/unix-def:broadcast_addr" priority="4000"
                 mode="M341">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the broadcast_addr entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M341"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M341"/>
   <xsl:template match="@*|node()" priority="-2" mode="M341">
      <xsl:apply-templates select="@*|node()" mode="M341"/>
   </xsl:template>

   <!--PATTERN unixinterfacestenetmask-->


	<!--RULE -->
<xsl:template match="unix-def:interface_state/unix-def:netmask" priority="4000" mode="M342">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the netmask entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M342"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M342"/>
   <xsl:template match="@*|node()" priority="-2" mode="M342">
      <xsl:apply-templates select="@*|node()" mode="M342"/>
   </xsl:template>

   <!--PATTERN unixinterfacesteflag-->


	<!--RULE -->
<xsl:template match="unix-def:interface_state/unix-def:flag" priority="4000" mode="M343">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for a flag entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M343"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M343"/>
   <xsl:template match="@*|node()" priority="-2" mode="M343">
      <xsl:apply-templates select="@*|node()" mode="M343"/>
   </xsl:template>

   <!--PATTERN passwordobjusername-->


	<!--RULE -->
<xsl:template match="unix-def:password_object/unix-def:username" priority="4000" mode="M344">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a password_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M344"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M344"/>
   <xsl:template match="@*|node()" priority="-2" mode="M344">
      <xsl:apply-templates select="@*|node()" mode="M344"/>
   </xsl:template>

   <!--PATTERN passwordsteusername-->


	<!--RULE -->
<xsl:template match="unix-def:package_state/unix-def:username" priority="4000" mode="M345">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a password_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M345"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M345"/>
   <xsl:template match="@*|node()" priority="-2" mode="M345">
      <xsl:apply-templates select="@*|node()" mode="M345"/>
   </xsl:template>

   <!--PATTERN passwordstepassword-->


	<!--RULE -->
<xsl:template match="unix-def:package_state/unix-def:password" priority="4000" mode="M346">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the password entity of a password_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M346"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M346"/>
   <xsl:template match="@*|node()" priority="-2" mode="M346">
      <xsl:apply-templates select="@*|node()" mode="M346"/>
   </xsl:template>

   <!--PATTERN passwordsteuser_id-->


	<!--RULE -->
<xsl:template match="unix-def:package_state/unix-def:user_id" priority="4000" mode="M347">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_id entity of a password_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M347"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M347"/>
   <xsl:template match="@*|node()" priority="-2" mode="M347">
      <xsl:apply-templates select="@*|node()" mode="M347"/>
   </xsl:template>

   <!--PATTERN passwordstegroup_id-->


	<!--RULE -->
<xsl:template match="unix-def:package_state/unix-def:group_id" priority="4000" mode="M348">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group_id entity of a password_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M348"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M348"/>
   <xsl:template match="@*|node()" priority="-2" mode="M348">
      <xsl:apply-templates select="@*|node()" mode="M348"/>
   </xsl:template>

   <!--PATTERN passwordstegcos-->


	<!--RULE -->
<xsl:template match="unix-def:package_state/unix-def:gcos" priority="4000" mode="M349">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the gcos entity of a password_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M349"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M349"/>
   <xsl:template match="@*|node()" priority="-2" mode="M349">
      <xsl:apply-templates select="@*|node()" mode="M349"/>
   </xsl:template>

   <!--PATTERN passwordstehome_dir-->


	<!--RULE -->
<xsl:template match="unix-def:package_state/unix-def:home_dir" priority="4000" mode="M350">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the home_dir entity of a password_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M350"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M350"/>
   <xsl:template match="@*|node()" priority="-2" mode="M350">
      <xsl:apply-templates select="@*|node()" mode="M350"/>
   </xsl:template>

   <!--PATTERN passwordstelogin_shell-->


	<!--RULE -->
<xsl:template match="unix-def:package_state/unix-def:login_shell" priority="4000" mode="M351">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the login_shell entity of a password_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M351"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M351"/>
   <xsl:template match="@*|node()" priority="-2" mode="M351">
      <xsl:apply-templates select="@*|node()" mode="M351"/>
   </xsl:template>

   <!--PATTERN unixprocessobjcommand-->


	<!--RULE -->
<xsl:template match="unix-def:process_object/unix-def:command" priority="4000" mode="M352">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the command entity of a process_object be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M352"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M352"/>
   <xsl:template match="@*|node()" priority="-2" mode="M352">
      <xsl:apply-templates select="@*|node()" mode="M352"/>
   </xsl:template>

   <!--PATTERN unixprocessstecommand-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:command" priority="4000" mode="M353">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the command entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M353"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M353"/>
   <xsl:template match="@*|node()" priority="-2" mode="M353">
      <xsl:apply-templates select="@*|node()" mode="M353"/>
   </xsl:template>

   <!--PATTERN unixprocesssteexec_time-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:exec_time" priority="4000" mode="M354">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the exec_time entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M354"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M354"/>
   <xsl:template match="@*|node()" priority="-2" mode="M354">
      <xsl:apply-templates select="@*|node()" mode="M354"/>
   </xsl:template>

   <!--PATTERN unixprocessstepid-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:pid" priority="4000" mode="M355">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pid entity of a process_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M355"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M355"/>
   <xsl:template match="@*|node()" priority="-2" mode="M355">
      <xsl:apply-templates select="@*|node()" mode="M355"/>
   </xsl:template>

   <!--PATTERN unixprocesssteppid-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:ppid" priority="4000" mode="M356">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ppid entity of a process_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M356"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M356"/>
   <xsl:template match="@*|node()" priority="-2" mode="M356">
      <xsl:apply-templates select="@*|node()" mode="M356"/>
   </xsl:template>

   <!--PATTERN unixprocessstepriority-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:priority" priority="4000" mode="M357">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the priority entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M357"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M357"/>
   <xsl:template match="@*|node()" priority="-2" mode="M357">
      <xsl:apply-templates select="@*|node()" mode="M357"/>
   </xsl:template>

   <!--PATTERN unixprocessstescheduling_class-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:scheduling_class" priority="4000"
                 mode="M358">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the scheduling_class entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M358"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M358"/>
   <xsl:template match="@*|node()" priority="-2" mode="M358">
      <xsl:apply-templates select="@*|node()" mode="M358"/>
   </xsl:template>

   <!--PATTERN unixprocessstestart_time-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:start_time" priority="4000" mode="M359">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the start_time entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M359"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M359"/>
   <xsl:template match="@*|node()" priority="-2" mode="M359">
      <xsl:apply-templates select="@*|node()" mode="M359"/>
   </xsl:template>

   <!--PATTERN unixprocessstetty-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:tty" priority="4000" mode="M360">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the tty entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M360"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M360"/>
   <xsl:template match="@*|node()" priority="-2" mode="M360">
      <xsl:apply-templates select="@*|node()" mode="M360"/>
   </xsl:template>

   <!--PATTERN unixprocesssteuser_id-->


	<!--RULE -->
<xsl:template match="unix-def:process_state/unix-def:user_id" priority="4000" mode="M361">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_id entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M361"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M361"/>
   <xsl:template match="@*|node()" priority="-2" mode="M361">
      <xsl:apply-templates select="@*|node()" mode="M361"/>
   </xsl:template>

   <!--PATTERN unixrlobjservice_name-->


	<!--RULE -->
<xsl:template match="unix-def:runlevel_object/unix-def:service_name" priority="4000"
                 mode="M362">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_name entity of a runlevel_object be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M362"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M362"/>
   <xsl:template match="@*|node()" priority="-2" mode="M362">
      <xsl:apply-templates select="@*|node()" mode="M362"/>
   </xsl:template>

   <!--PATTERN unixrlobjrunlevel-->


	<!--RULE -->
<xsl:template match="unix-def:runlevel_object/unix-def:runlevel" priority="4000" mode="M363">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the runlevel entity of a runlevel_object be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M363"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M363"/>
   <xsl:template match="@*|node()" priority="-2" mode="M363">
      <xsl:apply-templates select="@*|node()" mode="M363"/>
   </xsl:template>

   <!--PATTERN unixrlsteservice_name-->


	<!--RULE -->
<xsl:template match="unix-def:runlevel_state/unix-def:service_name" priority="4000"
                 mode="M364">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_name entity of a runlevel_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M364"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M364"/>
   <xsl:template match="@*|node()" priority="-2" mode="M364">
      <xsl:apply-templates select="@*|node()" mode="M364"/>
   </xsl:template>

   <!--PATTERN unixrlsterunlevel-->


	<!--RULE -->
<xsl:template match="unix-def:runlevel_state/unix-def:runlevel" priority="4000" mode="M365">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the runlevel entity of a runlevel_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M365"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M365"/>
   <xsl:template match="@*|node()" priority="-2" mode="M365">
      <xsl:apply-templates select="@*|node()" mode="M365"/>
   </xsl:template>

   <!--PATTERN unixrlstestart-->


	<!--RULE -->
<xsl:template match="unix-def:runlevel_state/unix-def:start" priority="4000" mode="M366">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the start entity of a runlevel_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M366"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M366"/>
   <xsl:template match="@*|node()" priority="-2" mode="M366">
      <xsl:apply-templates select="@*|node()" mode="M366"/>
   </xsl:template>

   <!--PATTERN unixrlstekill-->


	<!--RULE -->
<xsl:template match="unix-def:runlevel_state/unix-def:kill" priority="4000" mode="M367">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the kill entity of a runlevel_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M367"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M367"/>
   <xsl:template match="@*|node()" priority="-2" mode="M367">
      <xsl:apply-templates select="@*|node()" mode="M367"/>
   </xsl:template>

   <!--PATTERN sccsobjpath-->


	<!--RULE -->
<xsl:template match="unix-def:file_object/unix-def:path" priority="4000" mode="M368">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a sccs_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M368"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M368"/>
   <xsl:template match="@*|node()" priority="-2" mode="M368">
      <xsl:apply-templates select="@*|node()" mode="M368"/>
   </xsl:template>

   <!--PATTERN sccsobjfilename-->


	<!--RULE -->
<xsl:template match="unix-def:file_object/unix-def:filename" priority="4000" mode="M369">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a sccs_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M369"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M369"/>
   <xsl:template match="@*|node()" priority="-2" mode="M369">
      <xsl:apply-templates select="@*|node()" mode="M369"/>
   </xsl:template>

   <!--PATTERN sccsstepath-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:path" priority="4000" mode="M370">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M370"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M370"/>
   <xsl:template match="@*|node()" priority="-2" mode="M370">
      <xsl:apply-templates select="@*|node()" mode="M370"/>
   </xsl:template>

   <!--PATTERN sccsstefilename-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:filename" priority="4000" mode="M371">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M371"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M371"/>
   <xsl:template match="@*|node()" priority="-2" mode="M371">
      <xsl:apply-templates select="@*|node()" mode="M371"/>
   </xsl:template>

   <!--PATTERN sccsstemodule_name-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:module_name" priority="4000" mode="M372">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the module_name entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M372"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M372"/>
   <xsl:template match="@*|node()" priority="-2" mode="M372">
      <xsl:apply-templates select="@*|node()" mode="M372"/>
   </xsl:template>

   <!--PATTERN sccsstemodule_type-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:module_type" priority="4000" mode="M373">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the module_type entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M373"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M373"/>
   <xsl:template match="@*|node()" priority="-2" mode="M373">
      <xsl:apply-templates select="@*|node()" mode="M373"/>
   </xsl:template>

   <!--PATTERN sccssterelease-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:release" priority="4000" mode="M374">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the release entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M374"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M374"/>
   <xsl:template match="@*|node()" priority="-2" mode="M374">
      <xsl:apply-templates select="@*|node()" mode="M374"/>
   </xsl:template>

   <!--PATTERN sccsstelevel-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:level" priority="4000" mode="M375">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the level entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M375"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M375"/>
   <xsl:template match="@*|node()" priority="-2" mode="M375">
      <xsl:apply-templates select="@*|node()" mode="M375"/>
   </xsl:template>

   <!--PATTERN sccsstebranch-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:branch" priority="4000" mode="M376">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the branch entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M376"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M376"/>
   <xsl:template match="@*|node()" priority="-2" mode="M376">
      <xsl:apply-templates select="@*|node()" mode="M376"/>
   </xsl:template>

   <!--PATTERN sccsstesequence-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:sequence" priority="4000" mode="M377">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sequence entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M377"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M377"/>
   <xsl:template match="@*|node()" priority="-2" mode="M377">
      <xsl:apply-templates select="@*|node()" mode="M377"/>
   </xsl:template>

   <!--PATTERN sccsstewhat_string-->


	<!--RULE -->
<xsl:template match="unix-def:sccs_state/unix-def:what_string" priority="4000" mode="M378">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the what_string entity of a sccs_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M378"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M378"/>
   <xsl:template match="@*|node()" priority="-2" mode="M378">
      <xsl:apply-templates select="@*|node()" mode="M378"/>
   </xsl:template>

   <!--PATTERN shadowobjusername-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_object/unix-def:username" priority="4000" mode="M379">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a shadow_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M379"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M379"/>
   <xsl:template match="@*|node()" priority="-2" mode="M379">
      <xsl:apply-templates select="@*|node()" mode="M379"/>
   </xsl:template>

   <!--PATTERN shadowsteusername-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:username" priority="4000" mode="M380">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the username entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M380"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M380"/>
   <xsl:template match="@*|node()" priority="-2" mode="M380">
      <xsl:apply-templates select="@*|node()" mode="M380"/>
   </xsl:template>

   <!--PATTERN shadowstepassword-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:password" priority="4000" mode="M381">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the password entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M381"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M381"/>
   <xsl:template match="@*|node()" priority="-2" mode="M381">
      <xsl:apply-templates select="@*|node()" mode="M381"/>
   </xsl:template>

   <!--PATTERN shadowstechg_lst-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:chg_lst" priority="4000" mode="M382">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the chg_lst entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M382"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M382"/>
   <xsl:template match="@*|node()" priority="-2" mode="M382">
      <xsl:apply-templates select="@*|node()" mode="M382"/>
   </xsl:template>

   <!--PATTERN shadowstechg_allow-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:chg_allow" priority="4000" mode="M383">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the chg_allow entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M383"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M383"/>
   <xsl:template match="@*|node()" priority="-2" mode="M383">
      <xsl:apply-templates select="@*|node()" mode="M383"/>
   </xsl:template>

   <!--PATTERN shadowstechg_req-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:chg_req" priority="4000" mode="M384">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the chg_req entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M384"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M384"/>
   <xsl:template match="@*|node()" priority="-2" mode="M384">
      <xsl:apply-templates select="@*|node()" mode="M384"/>
   </xsl:template>

   <!--PATTERN shadowsteexp_warn-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:exp_warn" priority="4000" mode="M385">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the exp_warn entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M385"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M385"/>
   <xsl:template match="@*|node()" priority="-2" mode="M385">
      <xsl:apply-templates select="@*|node()" mode="M385"/>
   </xsl:template>

   <!--PATTERN shadowsteexp_inact-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:exp_inact" priority="4000" mode="M386">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the exp_inact entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M386"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M386"/>
   <xsl:template match="@*|node()" priority="-2" mode="M386">
      <xsl:apply-templates select="@*|node()" mode="M386"/>
   </xsl:template>

   <!--PATTERN shadowsteexp_date-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:exp_date" priority="4000" mode="M387">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the exp_date entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M387"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M387"/>
   <xsl:template match="@*|node()" priority="-2" mode="M387">
      <xsl:apply-templates select="@*|node()" mode="M387"/>
   </xsl:template>

   <!--PATTERN shadowsteflag-->


	<!--RULE -->
<xsl:template match="unix-def:shadow_state/unix-def:flag" priority="4000" mode="M388">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the flag entity of a shadow_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M388"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M388"/>
   <xsl:template match="@*|node()" priority="-2" mode="M388">
      <xsl:apply-templates select="@*|node()" mode="M388"/>
   </xsl:template>

   <!--PATTERN unamestemachine_class-->


	<!--RULE -->
<xsl:template match="unix-def:uname_state/unix-def:machine_class" priority="4000" mode="M389">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the machine_class entity of a uname_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M389"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M389"/>
   <xsl:template match="@*|node()" priority="-2" mode="M389">
      <xsl:apply-templates select="@*|node()" mode="M389"/>
   </xsl:template>

   <!--PATTERN unamestenode_name-->


	<!--RULE -->
<xsl:template match="unix-def:uname_state/unix-def:node_name" priority="4000" mode="M390">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the node_name entity of a uname_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M390"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M390"/>
   <xsl:template match="@*|node()" priority="-2" mode="M390">
      <xsl:apply-templates select="@*|node()" mode="M390"/>
   </xsl:template>

   <!--PATTERN unamesteos_name-->


	<!--RULE -->
<xsl:template match="unix-def:uname_state/unix-def:os_name" priority="4000" mode="M391">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the os_name entity of a uname_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M391"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M391"/>
   <xsl:template match="@*|node()" priority="-2" mode="M391">
      <xsl:apply-templates select="@*|node()" mode="M391"/>
   </xsl:template>

   <!--PATTERN unamesteos_release-->


	<!--RULE -->
<xsl:template match="unix-def:uname_state/unix-def:os_release" priority="4000" mode="M392">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the os_release entity of a uname_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M392"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M392"/>
   <xsl:template match="@*|node()" priority="-2" mode="M392">
      <xsl:apply-templates select="@*|node()" mode="M392"/>
   </xsl:template>

   <!--PATTERN unamesteos_version-->


	<!--RULE -->
<xsl:template match="unix-def:uname_state/unix-def:os_version" priority="4000" mode="M393">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the os_version entity of a uname_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M393"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M393"/>
   <xsl:template match="@*|node()" priority="-2" mode="M393">
      <xsl:apply-templates select="@*|node()" mode="M393"/>
   </xsl:template>

   <!--PATTERN unamesteprocessor_type-->


	<!--RULE -->
<xsl:template match="unix-def:uname_state/unix-def:processor_type" priority="4000"
                 mode="M394">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the processor_type entity of a uname_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M394"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M394"/>
   <xsl:template match="@*|node()" priority="-2" mode="M394">
      <xsl:apply-templates select="@*|node()" mode="M394"/>
   </xsl:template>

   <!--PATTERN xinetdobjprotocol-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_object/unix-def:protocol" priority="4000" mode="M395">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of an xinetd_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M395"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M395"/>
   <xsl:template match="@*|node()" priority="-2" mode="M395">
      <xsl:apply-templates select="@*|node()" mode="M395"/>
   </xsl:template>

   <!--PATTERN xinetdobjservice_name-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_object/unix-def:service_name" priority="4000"
                 mode="M396">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_name entity of an xinetd_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M396"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M396"/>
   <xsl:template match="@*|node()" priority="-2" mode="M396">
      <xsl:apply-templates select="@*|node()" mode="M396"/>
   </xsl:template>

   <!--PATTERN xinetdsteprotocol-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:protocol" priority="4000" mode="M397">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M397"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M397"/>
   <xsl:template match="@*|node()" priority="-2" mode="M397">
      <xsl:apply-templates select="@*|node()" mode="M397"/>
   </xsl:template>

   <!--PATTERN xinetdsteservice_name-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:service_name" priority="4000" mode="M398">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the service_name entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M398"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M398"/>
   <xsl:template match="@*|node()" priority="-2" mode="M398">
      <xsl:apply-templates select="@*|node()" mode="M398"/>
   </xsl:template>

   <!--PATTERN xinetdsteflags-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:flags" priority="4000" mode="M399">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the flags entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M399"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M399"/>
   <xsl:template match="@*|node()" priority="-2" mode="M399">
      <xsl:apply-templates select="@*|node()" mode="M399"/>
   </xsl:template>

   <!--PATTERN xinetdstenoaccess-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:no_access" priority="4000" mode="M400">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the no_access entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M400"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M400"/>
   <xsl:template match="@*|node()" priority="-2" mode="M400">
      <xsl:apply-templates select="@*|node()" mode="M400"/>
   </xsl:template>

   <!--PATTERN xinetdsteonlyfrom-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:only_from" priority="4000" mode="M401">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the only_from entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M401"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M401"/>
   <xsl:template match="@*|node()" priority="-2" mode="M401">
      <xsl:apply-templates select="@*|node()" mode="M401"/>
   </xsl:template>

   <!--PATTERN xinetdsteport-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:port" priority="4000" mode="M402">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the port entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M402"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M402"/>
   <xsl:template match="@*|node()" priority="-2" mode="M402">
      <xsl:apply-templates select="@*|node()" mode="M402"/>
   </xsl:template>

   <!--PATTERN xinetdsteserver-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:server" priority="4000" mode="M403">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the server entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M403"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M403"/>
   <xsl:template match="@*|node()" priority="-2" mode="M403">
      <xsl:apply-templates select="@*|node()" mode="M403"/>
   </xsl:template>

   <!--PATTERN xinetdsteserverarguments-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:server_arguments" priority="4000"
                 mode="M404">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the server_arguments entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M404"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M404"/>
   <xsl:template match="@*|node()" priority="-2" mode="M404">
      <xsl:apply-templates select="@*|node()" mode="M404"/>
   </xsl:template>

   <!--PATTERN xinetdstesockettype-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:socket_type" priority="4000" mode="M405">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the socket_type entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M405"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M405"/>
   <xsl:template match="@*|node()" priority="-2" mode="M405">
      <xsl:apply-templates select="@*|node()" mode="M405"/>
   </xsl:template>

   <!--PATTERN xinetdstetype-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:type" priority="4000" mode="M406">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the type entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M406"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M406"/>
   <xsl:template match="@*|node()" priority="-2" mode="M406">
      <xsl:apply-templates select="@*|node()" mode="M406"/>
   </xsl:template>

   <!--PATTERN xinetdsteuser-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:user" priority="4000" mode="M407">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user entity of an xinetd_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M407"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M407"/>
   <xsl:template match="@*|node()" priority="-2" mode="M407">
      <xsl:apply-templates select="@*|node()" mode="M407"/>
   </xsl:template>

   <!--PATTERN xinetdstewait-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:wait" priority="4000" mode="M408">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the wait entity of an xinetd_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M408"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M408"/>
   <xsl:template match="@*|node()" priority="-2" mode="M408">
      <xsl:apply-templates select="@*|node()" mode="M408"/>
   </xsl:template>

   <!--PATTERN xinetdstedisabled-->


	<!--RULE -->
<xsl:template match="unix-def:xinetd_state/unix-def:disabled" priority="4000" mode="M409">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the disabled entity of an xinetd_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M409"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M409"/>
   <xsl:template match="@*|node()" priority="-2" mode="M409">
      <xsl:apply-templates select="@*|node()" mode="M409"/>
   </xsl:template>

   <!--PATTERN affected_platform-->


	<!--RULE -->
<xsl:template match="oval-def:affected[@family='windows']" priority="4000" mode="M410">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(oval-def:platform) or oval-def:platform='Microsoft Windows 95' or oval-def:platform='Microsoft Windows 98' or oval-def:platform='Microsoft Windows ME' or oval-def:platform='Microsoft Windows NT' or oval-def:platform='Microsoft Windows 2000' or oval-def:platform='Microsoft Windows XP' or oval-def:platform='Microsoft Windows Server 2003' or oval-def:platform='Microsoft Windows Vista'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../../@id"/>
            <xsl:text/> - the value "<xsl:text/>
            <xsl:value-of select="oval-def:platform"/>
            <xsl:text/>" found in platform element as part of the affected element is not a valid windows platform.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M410"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M410"/>
   <xsl:template match="@*|node()" priority="-2" mode="M410">
      <xsl:apply-templates select="@*|node()" mode="M410"/>
   </xsl:template>

   <!--PATTERN attst-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_test/win-def:object" priority="4000" mode="M411">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:accesstoken_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an accesstoken_test must reference an accesstoken_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M411"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:accesstoken_test/win-def:state" priority="3999" mode="M411">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:accesstoken_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an accesstoken_test must reference an accesstoken_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M411"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M411"/>
   <xsl:template match="@*|node()" priority="-2" mode="M411">
      <xsl:apply-templates select="@*|node()" mode="M411"/>
   </xsl:template>

   <!--PATTERN atobjsecurity_principle-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_object/win-def:security_principle" priority="4000"
                 mode="M412">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the security_principle entity of an accesstoken_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M412"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M412"/>
   <xsl:template match="@*|node()" priority="-2" mode="M412">
      <xsl:apply-templates select="@*|node()" mode="M412"/>
   </xsl:template>

   <!--PATTERN atstesecurity_principle-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:security_principle" priority="4000"
                 mode="M413">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the security_principle entity of an accesstoken_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M413"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M413"/>
   <xsl:template match="@*|node()" priority="-2" mode="M413">
      <xsl:apply-templates select="@*|node()" mode="M413"/>
   </xsl:template>

   <!--PATTERN atsteseassignprimarytokenprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seassignprimarytokenprivilege"
                 priority="4000"
                 mode="M414">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seassignprimarytokenprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M414"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M414"/>
   <xsl:template match="@*|node()" priority="-2" mode="M414">
      <xsl:apply-templates select="@*|node()" mode="M414"/>
   </xsl:template>

   <!--PATTERN atsteseauditprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seauditprivilege" priority="4000"
                 mode="M415">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seauditprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M415"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M415"/>
   <xsl:template match="@*|node()" priority="-2" mode="M415">
      <xsl:apply-templates select="@*|node()" mode="M415"/>
   </xsl:template>

   <!--PATTERN atstesebackupprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sebackupprivilege" priority="4000"
                 mode="M416">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sebackupprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M416"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M416"/>
   <xsl:template match="@*|node()" priority="-2" mode="M416">
      <xsl:apply-templates select="@*|node()" mode="M416"/>
   </xsl:template>

   <!--PATTERN atstesechangenotifyprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sechangenotifyprivilege"
                 priority="4000"
                 mode="M417">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sechangenotifyprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M417"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M417"/>
   <xsl:template match="@*|node()" priority="-2" mode="M417">
      <xsl:apply-templates select="@*|node()" mode="M417"/>
   </xsl:template>

   <!--PATTERN atstesecreateglobalprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:secreateglobalprivilege"
                 priority="4000"
                 mode="M418">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the secreateglobalprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M418"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M418"/>
   <xsl:template match="@*|node()" priority="-2" mode="M418">
      <xsl:apply-templates select="@*|node()" mode="M418"/>
   </xsl:template>

   <!--PATTERN atstesecreatepagefileprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:secreatepagefileprivilege"
                 priority="4000"
                 mode="M419">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the secreatepagefileprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M419"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M419"/>
   <xsl:template match="@*|node()" priority="-2" mode="M419">
      <xsl:apply-templates select="@*|node()" mode="M419"/>
   </xsl:template>

   <!--PATTERN atstesecreatepermanentprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:secreatepermanentprivilege"
                 priority="4000"
                 mode="M420">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the secreatepermanentprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M420"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M420"/>
   <xsl:template match="@*|node()" priority="-2" mode="M420">
      <xsl:apply-templates select="@*|node()" mode="M420"/>
   </xsl:template>

   <!--PATTERN atstesecreatesymboliclinkprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:secreatesymboliclinkprivilege"
                 priority="4000"
                 mode="M421">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the secreatesymboliclinkprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M421"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M421"/>
   <xsl:template match="@*|node()" priority="-2" mode="M421">
      <xsl:apply-templates select="@*|node()" mode="M421"/>
   </xsl:template>

   <!--PATTERN atstesecreatetokenprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:secreatetokenprivilege"
                 priority="4000"
                 mode="M422">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the secreatetokenprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M422"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M422"/>
   <xsl:template match="@*|node()" priority="-2" mode="M422">
      <xsl:apply-templates select="@*|node()" mode="M422"/>
   </xsl:template>

   <!--PATTERN atstesedebugprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sedebugprivilege" priority="4000"
                 mode="M423">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sedebugprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M423"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M423"/>
   <xsl:template match="@*|node()" priority="-2" mode="M423">
      <xsl:apply-templates select="@*|node()" mode="M423"/>
   </xsl:template>

   <!--PATTERN atsteseenabledelegationprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seenabledelegationprivilege"
                 priority="4000"
                 mode="M424">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seenabledelegationprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M424"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M424"/>
   <xsl:template match="@*|node()" priority="-2" mode="M424">
      <xsl:apply-templates select="@*|node()" mode="M424"/>
   </xsl:template>

   <!--PATTERN atsteseimpersonateprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seimpersonateprivilege"
                 priority="4000"
                 mode="M425">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seimpersonateprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M425"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M425"/>
   <xsl:template match="@*|node()" priority="-2" mode="M425">
      <xsl:apply-templates select="@*|node()" mode="M425"/>
   </xsl:template>

   <!--PATTERN atsteseincreasebasepriorityprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seincreasebasepriorityprivilege"
                 priority="4000"
                 mode="M426">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seincreasebasepriorityprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M426"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M426"/>
   <xsl:template match="@*|node()" priority="-2" mode="M426">
      <xsl:apply-templates select="@*|node()" mode="M426"/>
   </xsl:template>

   <!--PATTERN atsteseincreasequotaprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seincreasequotaprivilege"
                 priority="4000"
                 mode="M427">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seincreasequotaprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M427"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M427"/>
   <xsl:template match="@*|node()" priority="-2" mode="M427">
      <xsl:apply-templates select="@*|node()" mode="M427"/>
   </xsl:template>

   <!--PATTERN atsteseincreaseworkingsetprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seincreaseworkingsetprivilege"
                 priority="4000"
                 mode="M428">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seincreaseworkingsetprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M428"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M428"/>
   <xsl:template match="@*|node()" priority="-2" mode="M428">
      <xsl:apply-templates select="@*|node()" mode="M428"/>
   </xsl:template>

   <!--PATTERN atsteseloaddriverprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seloaddriverprivilege" priority="4000"
                 mode="M429">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seloaddriverprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M429"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M429"/>
   <xsl:template match="@*|node()" priority="-2" mode="M429">
      <xsl:apply-templates select="@*|node()" mode="M429"/>
   </xsl:template>

   <!--PATTERN atsteselockmemoryprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:selockmemoryprivilege" priority="4000"
                 mode="M430">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the selockmemoryprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M430"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M430"/>
   <xsl:template match="@*|node()" priority="-2" mode="M430">
      <xsl:apply-templates select="@*|node()" mode="M430"/>
   </xsl:template>

   <!--PATTERN atstesemachineaccountprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:semachineaccountprivilege"
                 priority="4000"
                 mode="M431">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the semachineaccountprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M431"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M431"/>
   <xsl:template match="@*|node()" priority="-2" mode="M431">
      <xsl:apply-templates select="@*|node()" mode="M431"/>
   </xsl:template>

   <!--PATTERN atstesemanagevolumeprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:semanagevolumeprivilege"
                 priority="4000"
                 mode="M432">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the semanagevolumeprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M432"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M432"/>
   <xsl:template match="@*|node()" priority="-2" mode="M432">
      <xsl:apply-templates select="@*|node()" mode="M432"/>
   </xsl:template>

   <!--PATTERN atsteseprofilesingleprocessprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seprofilesingleprocessprivilege"
                 priority="4000"
                 mode="M433">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seprofilesingleprocessprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M433"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M433"/>
   <xsl:template match="@*|node()" priority="-2" mode="M433">
      <xsl:apply-templates select="@*|node()" mode="M433"/>
   </xsl:template>

   <!--PATTERN atsteserelabelprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:serelabelprivilege" priority="4000"
                 mode="M434">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the serelabelprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M434"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M434"/>
   <xsl:template match="@*|node()" priority="-2" mode="M434">
      <xsl:apply-templates select="@*|node()" mode="M434"/>
   </xsl:template>

   <!--PATTERN atsteseremoteshutdownprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seremoteshutdownprivilege"
                 priority="4000"
                 mode="M435">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seremoteshutdownprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M435"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M435"/>
   <xsl:template match="@*|node()" priority="-2" mode="M435">
      <xsl:apply-templates select="@*|node()" mode="M435"/>
   </xsl:template>

   <!--PATTERN atsteserestoreprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:serestoreprivilege" priority="4000"
                 mode="M436">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the serestoreprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M436"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M436"/>
   <xsl:template match="@*|node()" priority="-2" mode="M436">
      <xsl:apply-templates select="@*|node()" mode="M436"/>
   </xsl:template>

   <!--PATTERN atstesesecurityprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sesecurityprivilege" priority="4000"
                 mode="M437">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sesecurityprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M437"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M437"/>
   <xsl:template match="@*|node()" priority="-2" mode="M437">
      <xsl:apply-templates select="@*|node()" mode="M437"/>
   </xsl:template>

   <!--PATTERN atsteseshutdownprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seshutdownprivilege" priority="4000"
                 mode="M438">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seshutdownprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M438"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M438"/>
   <xsl:template match="@*|node()" priority="-2" mode="M438">
      <xsl:apply-templates select="@*|node()" mode="M438"/>
   </xsl:template>

   <!--PATTERN atstesesyncagentprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sesyncagentprivilege" priority="4000"
                 mode="M439">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sesyncagentprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M439"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M439"/>
   <xsl:template match="@*|node()" priority="-2" mode="M439">
      <xsl:apply-templates select="@*|node()" mode="M439"/>
   </xsl:template>

   <!--PATTERN atstesesystemenvironmentprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sesystemenvironmentprivilege"
                 priority="4000"
                 mode="M440">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sesystemenvironmentprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M440"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M440"/>
   <xsl:template match="@*|node()" priority="-2" mode="M440">
      <xsl:apply-templates select="@*|node()" mode="M440"/>
   </xsl:template>

   <!--PATTERN atstesesystemprofileprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sesystemprofileprivilege"
                 priority="4000"
                 mode="M441">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sesystemprofileprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M441"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M441"/>
   <xsl:template match="@*|node()" priority="-2" mode="M441">
      <xsl:apply-templates select="@*|node()" mode="M441"/>
   </xsl:template>

   <!--PATTERN atstesesystemtimeprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sesystemtimeprivilege" priority="4000"
                 mode="M442">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sesystemtimeprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M442"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M442"/>
   <xsl:template match="@*|node()" priority="-2" mode="M442">
      <xsl:apply-templates select="@*|node()" mode="M442"/>
   </xsl:template>

   <!--PATTERN atstesetakeownershipprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:setakeownershipprivilege"
                 priority="4000"
                 mode="M443">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the setakeownershipprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M443"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M443"/>
   <xsl:template match="@*|node()" priority="-2" mode="M443">
      <xsl:apply-templates select="@*|node()" mode="M443"/>
   </xsl:template>

   <!--PATTERN atstesetcbprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:setcbprivilege" priority="4000"
                 mode="M444">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the setcbprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M444"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M444"/>
   <xsl:template match="@*|node()" priority="-2" mode="M444">
      <xsl:apply-templates select="@*|node()" mode="M444"/>
   </xsl:template>

   <!--PATTERN atstesetimezoneprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:setimezoneprivilege" priority="4000"
                 mode="M445">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the setimezoneprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M445"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M445"/>
   <xsl:template match="@*|node()" priority="-2" mode="M445">
      <xsl:apply-templates select="@*|node()" mode="M445"/>
   </xsl:template>

   <!--PATTERN atsteseundockprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seundockprivilege" priority="4000"
                 mode="M446">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seundockprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M446"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M446"/>
   <xsl:template match="@*|node()" priority="-2" mode="M446">
      <xsl:apply-templates select="@*|node()" mode="M446"/>
   </xsl:template>

   <!--PATTERN atsteseunsolicitedinputprivilege-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seunsolicitedinputprivilege"
                 priority="4000"
                 mode="M447">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seunsolicitedinputprivilege entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M447"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M447"/>
   <xsl:template match="@*|node()" priority="-2" mode="M447">
      <xsl:apply-templates select="@*|node()" mode="M447"/>
   </xsl:template>

   <!--PATTERN atstesebatchlogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sebatchlogonright" priority="4000"
                 mode="M448">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sebatchlogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M448"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M448"/>
   <xsl:template match="@*|node()" priority="-2" mode="M448">
      <xsl:apply-templates select="@*|node()" mode="M448"/>
   </xsl:template>

   <!--PATTERN atsteseinteractivelogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seinteractivelogonright"
                 priority="4000"
                 mode="M449">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seinteractivelogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M449"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M449"/>
   <xsl:template match="@*|node()" priority="-2" mode="M449">
      <xsl:apply-templates select="@*|node()" mode="M449"/>
   </xsl:template>

   <!--PATTERN atstesenetworklogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:senetworklogonright" priority="4000"
                 mode="M450">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the senetworklogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M450"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M450"/>
   <xsl:template match="@*|node()" priority="-2" mode="M450">
      <xsl:apply-templates select="@*|node()" mode="M450"/>
   </xsl:template>

   <!--PATTERN atsteseremoteinteractivelogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seremoteinteractivelogonright"
                 priority="4000"
                 mode="M451">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seremoteinteractivelogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M451"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M451"/>
   <xsl:template match="@*|node()" priority="-2" mode="M451">
      <xsl:apply-templates select="@*|node()" mode="M451"/>
   </xsl:template>

   <!--PATTERN atsteseservicelogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:seservicelogonright" priority="4000"
                 mode="M452">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the seservicelogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M452"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M452"/>
   <xsl:template match="@*|node()" priority="-2" mode="M452">
      <xsl:apply-templates select="@*|node()" mode="M452"/>
   </xsl:template>

   <!--PATTERN atstesedenybatchLogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sedenybatchLogonright" priority="4000"
                 mode="M453">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sedenybatchLogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M453"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M453"/>
   <xsl:template match="@*|node()" priority="-2" mode="M453">
      <xsl:apply-templates select="@*|node()" mode="M453"/>
   </xsl:template>

   <!--PATTERN atstesedenyinteractivelogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sedenyinteractivelogonright"
                 priority="4000"
                 mode="M454">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sedenyinteractivelogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M454"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M454"/>
   <xsl:template match="@*|node()" priority="-2" mode="M454">
      <xsl:apply-templates select="@*|node()" mode="M454"/>
   </xsl:template>

   <!--PATTERN atstesedenynetworklogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sedenynetworklogonright"
                 priority="4000"
                 mode="M455">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sedenynetworklogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M455"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M455"/>
   <xsl:template match="@*|node()" priority="-2" mode="M455">
      <xsl:apply-templates select="@*|node()" mode="M455"/>
   </xsl:template>

   <!--PATTERN atstesedenyremoteInteractivelogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sedenyremoteInteractivelogonright"
                 priority="4000"
                 mode="M456">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sedenyremoteInteractivelogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M456"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M456"/>
   <xsl:template match="@*|node()" priority="-2" mode="M456">
      <xsl:apply-templates select="@*|node()" mode="M456"/>
   </xsl:template>

   <!--PATTERN atstesedenyservicelogonright-->


	<!--RULE -->
<xsl:template match="win-def:accesstoken_state/win-def:sedenyservicelogonright"
                 priority="4000"
                 mode="M457">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sedenyservicelogonright entity of an accesstoken_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M457"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M457"/>
   <xsl:template match="@*|node()" priority="-2" mode="M457">
      <xsl:apply-templates select="@*|node()" mode="M457"/>
   </xsl:template>

   <!--PATTERN adtst-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_test/win-def:object" priority="4000" mode="M458">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:activedirectory_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an activedirectory_test must reference an activedirectory_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M458"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:activedirectory_test/win-def:state" priority="3999" mode="M458">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:activedirectory_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an activedirectory_test must reference an activedirectory_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M458"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M458"/>
   <xsl:template match="@*|node()" priority="-2" mode="M458">
      <xsl:apply-templates select="@*|node()" mode="M458"/>
   </xsl:template>

   <!--PATTERN adobjnaming_context-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_object/win-def:naming_context" priority="4000"
                 mode="M459">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the naming_context entity of an activedirectory_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M459"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M459"/>
   <xsl:template match="@*|node()" priority="-2" mode="M459">
      <xsl:apply-templates select="@*|node()" mode="M459"/>
   </xsl:template>

   <!--PATTERN adobjrelative_dn-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_object/win-def:relative_dn" priority="4000"
                 mode="M460">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the relative_dn entity of an activedirectory_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@xsi:nil='true') or ../win-def:attribute/@xsi:nil='true'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - attribute entity must be nil when relative_dn is nil<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M460"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M460"/>
   <xsl:template match="@*|node()" priority="-2" mode="M460">
      <xsl:apply-templates select="@*|node()" mode="M460"/>
   </xsl:template>

   <!--PATTERN adobjattribute-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_object/win-def:attribute" priority="4000"
                 mode="M461">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the attribute entity of an activedirectory_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M461"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M461"/>
   <xsl:template match="@*|node()" priority="-2" mode="M461">
      <xsl:apply-templates select="@*|node()" mode="M461"/>
   </xsl:template>

   <!--PATTERN adstenaming_context-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_state/win-def:naming_context" priority="4000"
                 mode="M462">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the naming_context entity of an activedirectory_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M462"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M462"/>
   <xsl:template match="@*|node()" priority="-2" mode="M462">
      <xsl:apply-templates select="@*|node()" mode="M462"/>
   </xsl:template>

   <!--PATTERN adsterelative_dn-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_state/win-def:relative_dn" priority="4000"
                 mode="M463">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the relative_dn entity of an activedirectory_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M463"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M463"/>
   <xsl:template match="@*|node()" priority="-2" mode="M463">
      <xsl:apply-templates select="@*|node()" mode="M463"/>
   </xsl:template>

   <!--PATTERN adsteattribute-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_state/win-def:attribute" priority="4000"
                 mode="M464">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the attribute entity of an activedirectory_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M464"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M464"/>
   <xsl:template match="@*|node()" priority="-2" mode="M464">
      <xsl:apply-templates select="@*|node()" mode="M464"/>
   </xsl:template>

   <!--PATTERN adsteobject_class-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_state/win-def:object_class" priority="4000"
                 mode="M465">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the object_class entity of an activedirectory_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M465"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M465"/>
   <xsl:template match="@*|node()" priority="-2" mode="M465">
      <xsl:apply-templates select="@*|node()" mode="M465"/>
   </xsl:template>

   <!--PATTERN adsteadstype-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_state/win-def:adstype" priority="4000"
                 mode="M466">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the adstype entity of an activedirectory_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M466"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M466"/>
   <xsl:template match="@*|node()" priority="-2" mode="M466">
      <xsl:apply-templates select="@*|node()" mode="M466"/>
   </xsl:template>

   <!--PATTERN adstevalue-->


	<!--RULE -->
<xsl:template match="win-def:activedirectory_state/win-def:value" priority="4000" mode="M467">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M467"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M467"/>
   <xsl:template match="@*|node()" priority="-2" mode="M467">
      <xsl:apply-templates select="@*|node()" mode="M467"/>
   </xsl:template>

   <!--PATTERN aeptst-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_test/win-def:object" priority="4000"
                 mode="M468">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:auditeventpolicy_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an auditeventpolicy_test must reference an auditeventpolicy_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M468"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:auditeventpolicy_test/win-def:state" priority="3999" mode="M468">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:auditeventpolicy_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an auditeventpolicy_test must reference an auditeventpolicy_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M468"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M468"/>
   <xsl:template match="@*|node()" priority="-2" mode="M468">
      <xsl:apply-templates select="@*|node()" mode="M468"/>
   </xsl:template>

   <!--PATTERN aepsteaccount_logon-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:account_logon" priority="4000"
                 mode="M469">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the account_logon entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M469"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M469"/>
   <xsl:template match="@*|node()" priority="-2" mode="M469">
      <xsl:apply-templates select="@*|node()" mode="M469"/>
   </xsl:template>

   <!--PATTERN aepsteaccount_management-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:account_management"
                 priority="4000"
                 mode="M470">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the account_management entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M470"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M470"/>
   <xsl:template match="@*|node()" priority="-2" mode="M470">
      <xsl:apply-templates select="@*|node()" mode="M470"/>
   </xsl:template>

   <!--PATTERN aepstedetailed_tracking-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:detailed_tracking"
                 priority="4000"
                 mode="M471">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the detailed_tracking entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M471"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M471"/>
   <xsl:template match="@*|node()" priority="-2" mode="M471">
      <xsl:apply-templates select="@*|node()" mode="M471"/>
   </xsl:template>

   <!--PATTERN aepstedirectory_service_access-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:directory_service_access"
                 priority="4000"
                 mode="M472">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the directory_service_access entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M472"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M472"/>
   <xsl:template match="@*|node()" priority="-2" mode="M472">
      <xsl:apply-templates select="@*|node()" mode="M472"/>
   </xsl:template>

   <!--PATTERN aepstelogon-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:logon" priority="4000"
                 mode="M473">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the logon entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M473"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M473"/>
   <xsl:template match="@*|node()" priority="-2" mode="M473">
      <xsl:apply-templates select="@*|node()" mode="M473"/>
   </xsl:template>

   <!--PATTERN aepsteobject_access-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:object_access" priority="4000"
                 mode="M474">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the object_access entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M474"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M474"/>
   <xsl:template match="@*|node()" priority="-2" mode="M474">
      <xsl:apply-templates select="@*|node()" mode="M474"/>
   </xsl:template>

   <!--PATTERN aepstepolicy_change-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:policy_change" priority="4000"
                 mode="M475">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the policy_change entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M475"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M475"/>
   <xsl:template match="@*|node()" priority="-2" mode="M475">
      <xsl:apply-templates select="@*|node()" mode="M475"/>
   </xsl:template>

   <!--PATTERN aepsteprivilege_use-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:privilege_use" priority="4000"
                 mode="M476">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the privilege_use entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M476"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M476"/>
   <xsl:template match="@*|node()" priority="-2" mode="M476">
      <xsl:apply-templates select="@*|node()" mode="M476"/>
   </xsl:template>

   <!--PATTERN aepstesystem-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicy_state/win-def:system" priority="4000"
                 mode="M477">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the system entity of an auditeventpolicy_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M477"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M477"/>
   <xsl:template match="@*|node()" priority="-2" mode="M477">
      <xsl:apply-templates select="@*|node()" mode="M477"/>
   </xsl:template>

   <!--PATTERN aepstst-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_test/win-def:object"
                 priority="4000"
                 mode="M478">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:auditeventpolicysubcategories_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an auditeventpolicysubcategories_test must reference an auditeventpolicysubcategories_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M478"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_test/win-def:state"
                 priority="3999"
                 mode="M478">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:auditeventpolicysubcategories_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an auditeventpolicysubcategories_test must reference an auditeventpolicysubcategories_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M478"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M478"/>
   <xsl:template match="@*|node()" priority="-2" mode="M478">
      <xsl:apply-templates select="@*|node()" mode="M478"/>
   </xsl:template>

   <!--PATTERN aepsstecredentialvalidation-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:credential_validation"
                 priority="4000"
                 mode="M479">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the credential_validation entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M479"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M479"/>
   <xsl:template match="@*|node()" priority="-2" mode="M479">
      <xsl:apply-templates select="@*|node()" mode="M479"/>
   </xsl:template>

   <!--PATTERN aepsstekerberosticketevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:kerberos_ticket_events"
                 priority="4000"
                 mode="M480">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the kerberos_ticket_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M480"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M480"/>
   <xsl:template match="@*|node()" priority="-2" mode="M480">
      <xsl:apply-templates select="@*|node()" mode="M480"/>
   </xsl:template>

   <!--PATTERN aepssteotheraccountlogonevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_account_logon_events"
                 priority="4000"
                 mode="M481">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the other_account_logon_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M481"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M481"/>
   <xsl:template match="@*|node()" priority="-2" mode="M481">
      <xsl:apply-templates select="@*|node()" mode="M481"/>
   </xsl:template>

   <!--PATTERN aepssteapplicationgroupmanagement-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:application_group_management"
                 priority="4000"
                 mode="M482">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the application_group_management entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M482"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M482"/>
   <xsl:template match="@*|node()" priority="-2" mode="M482">
      <xsl:apply-templates select="@*|node()" mode="M482"/>
   </xsl:template>

   <!--PATTERN aepsstecomputeraccountmanagement-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:computer_account_management"
                 priority="4000"
                 mode="M483">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the computer_account_management entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M483"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M483"/>
   <xsl:template match="@*|node()" priority="-2" mode="M483">
      <xsl:apply-templates select="@*|node()" mode="M483"/>
   </xsl:template>

   <!--PATTERN aepsstedistributiongroupmanagement-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:distribution_group_management"
                 priority="4000"
                 mode="M484">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the distribution_group_management entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M484"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M484"/>
   <xsl:template match="@*|node()" priority="-2" mode="M484">
      <xsl:apply-templates select="@*|node()" mode="M484"/>
   </xsl:template>

   <!--PATTERN aepssteotheraccountmanagementevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_account_management_events"
                 priority="4000"
                 mode="M485">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the other_account_management_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M485"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M485"/>
   <xsl:template match="@*|node()" priority="-2" mode="M485">
      <xsl:apply-templates select="@*|node()" mode="M485"/>
   </xsl:template>

   <!--PATTERN aepsstesecuritygroupmanagement-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:security_group_management"
                 priority="4000"
                 mode="M486">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the security_group_management entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M486"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M486"/>
   <xsl:template match="@*|node()" priority="-2" mode="M486">
      <xsl:apply-templates select="@*|node()" mode="M486"/>
   </xsl:template>

   <!--PATTERN aepssteuseraccountmanagement-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:user_account_management"
                 priority="4000"
                 mode="M487">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_account_management entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M487"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M487"/>
   <xsl:template match="@*|node()" priority="-2" mode="M487">
      <xsl:apply-templates select="@*|node()" mode="M487"/>
   </xsl:template>

   <!--PATTERN aepsstedpapiactivity-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:dpapi_activity"
                 priority="4000"
                 mode="M488">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the dpapi_activity entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M488"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M488"/>
   <xsl:template match="@*|node()" priority="-2" mode="M488">
      <xsl:apply-templates select="@*|node()" mode="M488"/>
   </xsl:template>

   <!--PATTERN aepssteprocesscreation-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:process_creation"
                 priority="4000"
                 mode="M489">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the process_creation entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M489"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M489"/>
   <xsl:template match="@*|node()" priority="-2" mode="M489">
      <xsl:apply-templates select="@*|node()" mode="M489"/>
   </xsl:template>

   <!--PATTERN aepssteprocesstermination-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:process_termination"
                 priority="4000"
                 mode="M490">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the process_termination entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M490"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M490"/>
   <xsl:template match="@*|node()" priority="-2" mode="M490">
      <xsl:apply-templates select="@*|node()" mode="M490"/>
   </xsl:template>

   <!--PATTERN aepssterpcevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:rpc_events"
                 priority="4000"
                 mode="M491">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the rpc_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M491"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M491"/>
   <xsl:template match="@*|node()" priority="-2" mode="M491">
      <xsl:apply-templates select="@*|node()" mode="M491"/>
   </xsl:template>

   <!--PATTERN aepsstedirectoryserviceaccess-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:directory_service_access"
                 priority="4000"
                 mode="M492">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the directory_service_access entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M492"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M492"/>
   <xsl:template match="@*|node()" priority="-2" mode="M492">
      <xsl:apply-templates select="@*|node()" mode="M492"/>
   </xsl:template>

   <!--PATTERN aepsstedirectoryservicechanges-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:directory_service_changes"
                 priority="4000"
                 mode="M493">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the directory_service_changes entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M493"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M493"/>
   <xsl:template match="@*|node()" priority="-2" mode="M493">
      <xsl:apply-templates select="@*|node()" mode="M493"/>
   </xsl:template>

   <!--PATTERN aepsstedirectoryservicereplication-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:directory_service_replication"
                 priority="4000"
                 mode="M494">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the directory_service_replication entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M494"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M494"/>
   <xsl:template match="@*|node()" priority="-2" mode="M494">
      <xsl:apply-templates select="@*|node()" mode="M494"/>
   </xsl:template>

   <!--PATTERN aepsstedetaileddirectoryservicereplication-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:detailed_directory_service_replication"
                 priority="4000"
                 mode="M495">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the detailed_directory_service_replication entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M495"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M495"/>
   <xsl:template match="@*|node()" priority="-2" mode="M495">
      <xsl:apply-templates select="@*|node()" mode="M495"/>
   </xsl:template>

   <!--PATTERN aepssteaccountlockout-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:account_lockout"
                 priority="4000"
                 mode="M496">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the account_lockout entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M496"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M496"/>
   <xsl:template match="@*|node()" priority="-2" mode="M496">
      <xsl:apply-templates select="@*|node()" mode="M496"/>
   </xsl:template>

   <!--PATTERN aepssteipsecextendedmode-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_extended_mode"
                 priority="4000"
                 mode="M497">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ipsec_extended_mode entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M497"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M497"/>
   <xsl:template match="@*|node()" priority="-2" mode="M497">
      <xsl:apply-templates select="@*|node()" mode="M497"/>
   </xsl:template>

   <!--PATTERN aepssteipsecmainmode-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_main_mode"
                 priority="4000"
                 mode="M498">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ipsec_main_mode entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M498"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M498"/>
   <xsl:template match="@*|node()" priority="-2" mode="M498">
      <xsl:apply-templates select="@*|node()" mode="M498"/>
   </xsl:template>

   <!--PATTERN aepssteipsec_quick_mode-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_quick_mode"
                 priority="4000"
                 mode="M499">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ipsec_quick_mode entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M499"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M499"/>
   <xsl:template match="@*|node()" priority="-2" mode="M499">
      <xsl:apply-templates select="@*|node()" mode="M499"/>
   </xsl:template>

   <!--PATTERN aepsstelogoff-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:logoff"
                 priority="4000"
                 mode="M500">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the logoff entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M500"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M500"/>
   <xsl:template match="@*|node()" priority="-2" mode="M500">
      <xsl:apply-templates select="@*|node()" mode="M500"/>
   </xsl:template>

   <!--PATTERN aepsstelogon-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:logon"
                 priority="4000"
                 mode="M501">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the logon entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M501"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M501"/>
   <xsl:template match="@*|node()" priority="-2" mode="M501">
      <xsl:apply-templates select="@*|node()" mode="M501"/>
   </xsl:template>

   <!--PATTERN aepssteotherlogonlogoffevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_logon_logoff_events"
                 priority="4000"
                 mode="M502">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the other_logon_logoff_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M502"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M502"/>
   <xsl:template match="@*|node()" priority="-2" mode="M502">
      <xsl:apply-templates select="@*|node()" mode="M502"/>
   </xsl:template>

   <!--PATTERN aepsstespeciallogon-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:special_logon"
                 priority="4000"
                 mode="M503">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the special_logon entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M503"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M503"/>
   <xsl:template match="@*|node()" priority="-2" mode="M503">
      <xsl:apply-templates select="@*|node()" mode="M503"/>
   </xsl:template>

   <!--PATTERN aepssteapplicationgenerated-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:application_generated"
                 priority="4000"
                 mode="M504">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the application_generated entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M504"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M504"/>
   <xsl:template match="@*|node()" priority="-2" mode="M504">
      <xsl:apply-templates select="@*|node()" mode="M504"/>
   </xsl:template>

   <!--PATTERN aepsstecertificationservices-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:certification_services"
                 priority="4000"
                 mode="M505">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the certification_services entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M505"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M505"/>
   <xsl:template match="@*|node()" priority="-2" mode="M505">
      <xsl:apply-templates select="@*|node()" mode="M505"/>
   </xsl:template>

   <!--PATTERN aepsstefileshare-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:file_share"
                 priority="4000"
                 mode="M506">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_share entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M506"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M506"/>
   <xsl:template match="@*|node()" priority="-2" mode="M506">
      <xsl:apply-templates select="@*|node()" mode="M506"/>
   </xsl:template>

   <!--PATTERN aepsstefilesystem-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:file_system"
                 priority="4000"
                 mode="M507">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_system entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M507"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M507"/>
   <xsl:template match="@*|node()" priority="-2" mode="M507">
      <xsl:apply-templates select="@*|node()" mode="M507"/>
   </xsl:template>

   <!--PATTERN aepsstefilteringplatformconnection-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:filtering_platform_connection"
                 priority="4000"
                 mode="M508">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filtering_platform_connection entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M508"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M508"/>
   <xsl:template match="@*|node()" priority="-2" mode="M508">
      <xsl:apply-templates select="@*|node()" mode="M508"/>
   </xsl:template>

   <!--PATTERN aepsstefilteringplatformpacketdrop-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:filtering_platform_packet_drop"
                 priority="4000"
                 mode="M509">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filtering_platform_packet_drop entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M509"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M509"/>
   <xsl:template match="@*|node()" priority="-2" mode="M509">
      <xsl:apply-templates select="@*|node()" mode="M509"/>
   </xsl:template>

   <!--PATTERN aepsstehandlemanipulation-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:handle_manipulation"
                 priority="4000"
                 mode="M510">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the handle_manipulation entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M510"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M510"/>
   <xsl:template match="@*|node()" priority="-2" mode="M510">
      <xsl:apply-templates select="@*|node()" mode="M510"/>
   </xsl:template>

   <!--PATTERN aepsstekernelobject-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:kernel_object"
                 priority="4000"
                 mode="M511">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the kernel_object entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M511"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M511"/>
   <xsl:template match="@*|node()" priority="-2" mode="M511">
      <xsl:apply-templates select="@*|node()" mode="M511"/>
   </xsl:template>

   <!--PATTERN aepssteotherobjectaccessevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_object_access_events"
                 priority="4000"
                 mode="M512">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the other_object_access_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M512"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M512"/>
   <xsl:template match="@*|node()" priority="-2" mode="M512">
      <xsl:apply-templates select="@*|node()" mode="M512"/>
   </xsl:template>

   <!--PATTERN aepssteregistry-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:registry"
                 priority="4000"
                 mode="M513">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the registry entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M513"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M513"/>
   <xsl:template match="@*|node()" priority="-2" mode="M513">
      <xsl:apply-templates select="@*|node()" mode="M513"/>
   </xsl:template>

   <!--PATTERN aepsstesam-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:sam" priority="4000"
                 mode="M514">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sam entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M514"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M514"/>
   <xsl:template match="@*|node()" priority="-2" mode="M514">
      <xsl:apply-templates select="@*|node()" mode="M514"/>
   </xsl:template>

   <!--PATTERN aepssteauditpolicychange-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:audit_policy_change"
                 priority="4000"
                 mode="M515">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the audit_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M515"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M515"/>
   <xsl:template match="@*|node()" priority="-2" mode="M515">
      <xsl:apply-templates select="@*|node()" mode="M515"/>
   </xsl:template>

   <!--PATTERN aepssteauthenticationpolicychange-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:authentication_policy_change"
                 priority="4000"
                 mode="M516">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the authentication_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M516"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M516"/>
   <xsl:template match="@*|node()" priority="-2" mode="M516">
      <xsl:apply-templates select="@*|node()" mode="M516"/>
   </xsl:template>

   <!--PATTERN aepssteauthorizationpolicychange-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:authorization_policy_change"
                 priority="4000"
                 mode="M517">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the authorization_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M517"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M517"/>
   <xsl:template match="@*|node()" priority="-2" mode="M517">
      <xsl:apply-templates select="@*|node()" mode="M517"/>
   </xsl:template>

   <!--PATTERN aepsstefilteringplatformpolicychange-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:filtering_platform_policy_change"
                 priority="4000"
                 mode="M518">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filtering_platform_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M518"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M518"/>
   <xsl:template match="@*|node()" priority="-2" mode="M518">
      <xsl:apply-templates select="@*|node()" mode="M518"/>
   </xsl:template>

   <!--PATTERN aepsstempssvcrulelevelpolicychange-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:mpssvc_rule_level_policy_change"
                 priority="4000"
                 mode="M519">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the mpssvc_rule_level_policy_change entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M519"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M519"/>
   <xsl:template match="@*|node()" priority="-2" mode="M519">
      <xsl:apply-templates select="@*|node()" mode="M519"/>
   </xsl:template>

   <!--PATTERN aepssteotherpolicychangeevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_policy_change_events"
                 priority="4000"
                 mode="M520">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the other_policy_change_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M520"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M520"/>
   <xsl:template match="@*|node()" priority="-2" mode="M520">
      <xsl:apply-templates select="@*|node()" mode="M520"/>
   </xsl:template>

   <!--PATTERN aepsstenonsensitiveprivilegeuse-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:non_sensitive_privilege_use"
                 priority="4000"
                 mode="M521">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the non_sensitive_privilege_use entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M521"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M521"/>
   <xsl:template match="@*|node()" priority="-2" mode="M521">
      <xsl:apply-templates select="@*|node()" mode="M521"/>
   </xsl:template>

   <!--PATTERN aepssteotherprivilegeuseevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_privilege_use_events"
                 priority="4000"
                 mode="M522">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the other_privilege_use_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M522"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M522"/>
   <xsl:template match="@*|node()" priority="-2" mode="M522">
      <xsl:apply-templates select="@*|node()" mode="M522"/>
   </xsl:template>

   <!--PATTERN aepsstesensitive_privilege_use-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:sensitive_privilege_use"
                 priority="4000"
                 mode="M523">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the sensitive_privilege_use entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M523"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M523"/>
   <xsl:template match="@*|node()" priority="-2" mode="M523">
      <xsl:apply-templates select="@*|node()" mode="M523"/>
   </xsl:template>

   <!--PATTERN aepssteipsecdriver-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:ipsec_driver"
                 priority="4000"
                 mode="M524">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ipsec_driver entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M524"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M524"/>
   <xsl:template match="@*|node()" priority="-2" mode="M524">
      <xsl:apply-templates select="@*|node()" mode="M524"/>
   </xsl:template>

   <!--PATTERN aepssteothersystemevents-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:other_system_events"
                 priority="4000"
                 mode="M525">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the other_system_events entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M525"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M525"/>
   <xsl:template match="@*|node()" priority="-2" mode="M525">
      <xsl:apply-templates select="@*|node()" mode="M525"/>
   </xsl:template>

   <!--PATTERN aepsstesecuritystatechange-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:security_state_change"
                 priority="4000"
                 mode="M526">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the security_state_change entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M526"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M526"/>
   <xsl:template match="@*|node()" priority="-2" mode="M526">
      <xsl:apply-templates select="@*|node()" mode="M526"/>
   </xsl:template>

   <!--PATTERN aepsstesecuritysystemextension-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:security_system_extension"
                 priority="4000"
                 mode="M527">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the security_system_extension entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M527"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M527"/>
   <xsl:template match="@*|node()" priority="-2" mode="M527">
      <xsl:apply-templates select="@*|node()" mode="M527"/>
   </xsl:template>

   <!--PATTERN aepsstesystemintegrity-->


	<!--RULE -->
<xsl:template match="win-def:auditeventpolicysubcategories_state/win-def:system_integrity"
                 priority="4000"
                 mode="M528">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the system_integrity entity of an auditeventpolicysubcategories_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M528"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M528"/>
   <xsl:template match="@*|node()" priority="-2" mode="M528">
      <xsl:apply-templates select="@*|node()" mode="M528"/>
   </xsl:template>

   <!--PATTERN filetst-->


	<!--RULE -->
<xsl:template match="win-def:file_test/win-def:object" priority="4000" mode="M529">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:file_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a file_test must reference a file_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M529"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:file_test/win-def:state" priority="3999" mode="M529">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:file_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a file_test must reference a file_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M529"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M529"/>
   <xsl:template match="@*|node()" priority="-2" mode="M529">
      <xsl:apply-templates select="@*|node()" mode="M529"/>
   </xsl:template>

   <!--PATTERN fileobjpath-->


	<!--RULE -->
<xsl:template match="win-def:file_object/win-def:path" priority="4000" mode="M530">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a file_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M530"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M530"/>
   <xsl:template match="@*|node()" priority="-2" mode="M530">
      <xsl:apply-templates select="@*|node()" mode="M530"/>
   </xsl:template>

   <!--PATTERN fileobjfilename-->


	<!--RULE -->
<xsl:template match="win-def:file_object/win-def:filename" priority="4000" mode="M531">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a file_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M531"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M531"/>
   <xsl:template match="@*|node()" priority="-2" mode="M531">
      <xsl:apply-templates select="@*|node()" mode="M531"/>
   </xsl:template>

   <!--PATTERN filestepath-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:path" priority="4000" mode="M532">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M532"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M532"/>
   <xsl:template match="@*|node()" priority="-2" mode="M532">
      <xsl:apply-templates select="@*|node()" mode="M532"/>
   </xsl:template>

   <!--PATTERN filestefilename-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:filename" priority="4000" mode="M533">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M533"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M533"/>
   <xsl:template match="@*|node()" priority="-2" mode="M533">
      <xsl:apply-templates select="@*|node()" mode="M533"/>
   </xsl:template>

   <!--PATTERN filesteowner-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:owner" priority="4000" mode="M534">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the owner entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M534"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M534"/>
   <xsl:template match="@*|node()" priority="-2" mode="M534">
      <xsl:apply-templates select="@*|node()" mode="M534"/>
   </xsl:template>

   <!--PATTERN filestesize-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:size" priority="4000" mode="M535">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the size entity of a file_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M535"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M535"/>
   <xsl:template match="@*|node()" priority="-2" mode="M535">
      <xsl:apply-templates select="@*|node()" mode="M535"/>
   </xsl:template>

   <!--PATTERN filestea_time-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:a_time" priority="4000" mode="M536">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the a_time entity of a file_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M536"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M536"/>
   <xsl:template match="@*|node()" priority="-2" mode="M536">
      <xsl:apply-templates select="@*|node()" mode="M536"/>
   </xsl:template>

   <!--PATTERN filestec_time-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:c_time" priority="4000" mode="M537">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the c_time entity of a file_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M537"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M537"/>
   <xsl:template match="@*|node()" priority="-2" mode="M537">
      <xsl:apply-templates select="@*|node()" mode="M537"/>
   </xsl:template>

   <!--PATTERN filestem_time-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:m_time" priority="4000" mode="M538">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the m_time entity of a file_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M538"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M538"/>
   <xsl:template match="@*|node()" priority="-2" mode="M538">
      <xsl:apply-templates select="@*|node()" mode="M538"/>
   </xsl:template>

   <!--PATTERN filestems_checksum-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:ms_checksum" priority="4000" mode="M539">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ms_checksum entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M539"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M539"/>
   <xsl:template match="@*|node()" priority="-2" mode="M539">
      <xsl:apply-templates select="@*|node()" mode="M539"/>
   </xsl:template>

   <!--PATTERN filesteversion-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:version" priority="4000" mode="M540">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the version entity of a file_state should be 'version'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M540"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M540"/>
   <xsl:template match="@*|node()" priority="-2" mode="M540">
      <xsl:apply-templates select="@*|node()" mode="M540"/>
   </xsl:template>

   <!--PATTERN filestetype-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:type" priority="4000" mode="M541">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the type entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M541"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M541"/>
   <xsl:template match="@*|node()" priority="-2" mode="M541">
      <xsl:apply-templates select="@*|node()" mode="M541"/>
   </xsl:template>

   <!--PATTERN filestedevelopment_class-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:development_class" priority="4000"
                 mode="M542">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the development_class entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M542"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M542"/>
   <xsl:template match="@*|node()" priority="-2" mode="M542">
      <xsl:apply-templates select="@*|node()" mode="M542"/>
   </xsl:template>

   <!--PATTERN filestecompany-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:company" priority="4000" mode="M543">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the company entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M543"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M543"/>
   <xsl:template match="@*|node()" priority="-2" mode="M543">
      <xsl:apply-templates select="@*|node()" mode="M543"/>
   </xsl:template>

   <!--PATTERN filesteinternalname-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:internal_name" priority="4000" mode="M544">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the internal_name entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M544"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M544"/>
   <xsl:template match="@*|node()" priority="-2" mode="M544">
      <xsl:apply-templates select="@*|node()" mode="M544"/>
   </xsl:template>

   <!--PATTERN filestelanguage-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:language" priority="4000" mode="M545">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the language entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M545"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M545"/>
   <xsl:template match="@*|node()" priority="-2" mode="M545">
      <xsl:apply-templates select="@*|node()" mode="M545"/>
   </xsl:template>

   <!--PATTERN filesteoriginalfilename-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:original_filename" priority="4000"
                 mode="M546">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the original_filename entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M546"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M546"/>
   <xsl:template match="@*|node()" priority="-2" mode="M546">
      <xsl:apply-templates select="@*|node()" mode="M546"/>
   </xsl:template>

   <!--PATTERN filesteproductname-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:product_name" priority="4000" mode="M547">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the product_name entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M547"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M547"/>
   <xsl:template match="@*|node()" priority="-2" mode="M547">
      <xsl:apply-templates select="@*|node()" mode="M547"/>
   </xsl:template>

   <!--PATTERN filesteproductversion-->


	<!--RULE -->
<xsl:template match="win-def:file_state/win-def:product_version" priority="4000" mode="M548">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='version'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the product_version entity of a file_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M548"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M548"/>
   <xsl:template match="@*|node()" priority="-2" mode="M548">
      <xsl:apply-templates select="@*|node()" mode="M548"/>
   </xsl:template>

   <!--PATTERN fap53tst-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_test/win-def:object" priority="4000"
                 mode="M549">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions53_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a fileauditedpermissions53_test must reference a fileauditedpermissions53_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M549"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_test/win-def:state" priority="3999"
                 mode="M549">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileauditedpermissions53_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a fileauditedpermissions53_test must reference a fileauditedpermissions53_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M549"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M549"/>
   <xsl:template match="@*|node()" priority="-2" mode="M549">
      <xsl:apply-templates select="@*|node()" mode="M549"/>
   </xsl:template>

   <!--PATTERN fap53objpath-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_object/win-def:path" priority="4000"
                 mode="M550">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileauditedpermissions53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M550"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M550"/>
   <xsl:template match="@*|node()" priority="-2" mode="M550">
      <xsl:apply-templates select="@*|node()" mode="M550"/>
   </xsl:template>

   <!--PATTERN fap53objfilename-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_object/win-def:filename"
                 priority="4000"
                 mode="M551">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M551"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M551"/>
   <xsl:template match="@*|node()" priority="-2" mode="M551">
      <xsl:apply-templates select="@*|node()" mode="M551"/>
   </xsl:template>

   <!--PATTERN fap53objtrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_object/win-def:trustee_sid"
                 priority="4000"
                 mode="M552">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a fileauditedpermissions53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M552"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M552"/>
   <xsl:template match="@*|node()" priority="-2" mode="M552">
      <xsl:apply-templates select="@*|node()" mode="M552"/>
   </xsl:template>

   <!--PATTERN fap53stepath-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:path" priority="4000"
                 mode="M553">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M553"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M553"/>
   <xsl:template match="@*|node()" priority="-2" mode="M553">
      <xsl:apply-templates select="@*|node()" mode="M553"/>
   </xsl:template>

   <!--PATTERN fap53stefilename-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:filename" priority="4000"
                 mode="M554">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M554"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M554"/>
   <xsl:template match="@*|node()" priority="-2" mode="M554">
      <xsl:apply-templates select="@*|node()" mode="M554"/>
   </xsl:template>

   <!--PATTERN fap53stetrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:trustee_sid"
                 priority="4000"
                 mode="M555">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M555"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M555"/>
   <xsl:template match="@*|node()" priority="-2" mode="M555">
      <xsl:apply-templates select="@*|node()" mode="M555"/>
   </xsl:template>

   <!--PATTERN fap53stestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_delete"
                 priority="4000"
                 mode="M556">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M556"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M556"/>
   <xsl:template match="@*|node()" priority="-2" mode="M556">
      <xsl:apply-templates select="@*|node()" mode="M556"/>
   </xsl:template>

   <!--PATTERN fap53stestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M557">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M557"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M557"/>
   <xsl:template match="@*|node()" priority="-2" mode="M557">
      <xsl:apply-templates select="@*|node()" mode="M557"/>
   </xsl:template>

   <!--PATTERN fap53stestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M558">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M558"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M558"/>
   <xsl:template match="@*|node()" priority="-2" mode="M558">
      <xsl:apply-templates select="@*|node()" mode="M558"/>
   </xsl:template>

   <!--PATTERN fap53stestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M559">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M559"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M559"/>
   <xsl:template match="@*|node()" priority="-2" mode="M559">
      <xsl:apply-templates select="@*|node()" mode="M559"/>
   </xsl:template>

   <!--PATTERN fap53stestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M560">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M560"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M560"/>
   <xsl:template match="@*|node()" priority="-2" mode="M560">
      <xsl:apply-templates select="@*|node()" mode="M560"/>
   </xsl:template>

   <!--PATTERN fap53steaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:access_system_security"
                 priority="4000"
                 mode="M561">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M561"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M561"/>
   <xsl:template match="@*|node()" priority="-2" mode="M561">
      <xsl:apply-templates select="@*|node()" mode="M561"/>
   </xsl:template>

   <!--PATTERN fap53stegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_read"
                 priority="4000"
                 mode="M562">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M562"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M562"/>
   <xsl:template match="@*|node()" priority="-2" mode="M562">
      <xsl:apply-templates select="@*|node()" mode="M562"/>
   </xsl:template>

   <!--PATTERN fap53stegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_write"
                 priority="4000"
                 mode="M563">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M563"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M563"/>
   <xsl:template match="@*|node()" priority="-2" mode="M563">
      <xsl:apply-templates select="@*|node()" mode="M563"/>
   </xsl:template>

   <!--PATTERN fap53stegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_execute"
                 priority="4000"
                 mode="M564">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M564"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M564"/>
   <xsl:template match="@*|node()" priority="-2" mode="M564">
      <xsl:apply-templates select="@*|node()" mode="M564"/>
   </xsl:template>

   <!--PATTERN fap53stegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:generic_all"
                 priority="4000"
                 mode="M565">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M565"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M565"/>
   <xsl:template match="@*|node()" priority="-2" mode="M565">
      <xsl:apply-templates select="@*|node()" mode="M565"/>
   </xsl:template>

   <!--PATTERN fap53stefile_read_data-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_read_data"
                 priority="4000"
                 mode="M566">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_data entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M566"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M566"/>
   <xsl:template match="@*|node()" priority="-2" mode="M566">
      <xsl:apply-templates select="@*|node()" mode="M566"/>
   </xsl:template>

   <!--PATTERN fap53stefile_write_data-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_write_data"
                 priority="4000"
                 mode="M567">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_data entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M567"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M567"/>
   <xsl:template match="@*|node()" priority="-2" mode="M567">
      <xsl:apply-templates select="@*|node()" mode="M567"/>
   </xsl:template>

   <!--PATTERN fap53stefile_append_data-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_append_data"
                 priority="4000"
                 mode="M568">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_append_data entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M568"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M568"/>
   <xsl:template match="@*|node()" priority="-2" mode="M568">
      <xsl:apply-templates select="@*|node()" mode="M568"/>
   </xsl:template>

   <!--PATTERN fap53stefile_read_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_read_ea"
                 priority="4000"
                 mode="M569">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_ea entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M569"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M569"/>
   <xsl:template match="@*|node()" priority="-2" mode="M569">
      <xsl:apply-templates select="@*|node()" mode="M569"/>
   </xsl:template>

   <!--PATTERN fap53stefile_write_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_write_ea"
                 priority="4000"
                 mode="M570">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_ea entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M570"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M570"/>
   <xsl:template match="@*|node()" priority="-2" mode="M570">
      <xsl:apply-templates select="@*|node()" mode="M570"/>
   </xsl:template>

   <!--PATTERN fap53stefile_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_execute"
                 priority="4000"
                 mode="M571">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_execute entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M571"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M571"/>
   <xsl:template match="@*|node()" priority="-2" mode="M571">
      <xsl:apply-templates select="@*|node()" mode="M571"/>
   </xsl:template>

   <!--PATTERN fap53stefile_delete_child-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_delete_child"
                 priority="4000"
                 mode="M572">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_delete_child entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M572"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M572"/>
   <xsl:template match="@*|node()" priority="-2" mode="M572">
      <xsl:apply-templates select="@*|node()" mode="M572"/>
   </xsl:template>

   <!--PATTERN fap53stefile_read_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_read_attributes"
                 priority="4000"
                 mode="M573">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_attributes entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M573"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M573"/>
   <xsl:template match="@*|node()" priority="-2" mode="M573">
      <xsl:apply-templates select="@*|node()" mode="M573"/>
   </xsl:template>

   <!--PATTERN fap53stefile_write_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions53_state/win-def:file_write_attributes"
                 priority="4000"
                 mode="M574">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_attributes entity of a fileauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M574"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M574"/>
   <xsl:template match="@*|node()" priority="-2" mode="M574">
      <xsl:apply-templates select="@*|node()" mode="M574"/>
   </xsl:template>

   <!--PATTERN faptst-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_test/win-def:object" priority="4000"
                 mode="M575">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileauditedpermissions_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a fileauditedpermissions_test must reference a fileauditedpermissions_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M575"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_test/win-def:state" priority="3999"
                 mode="M575">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileauditedpermissions_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a fileauditedpermissions_test must reference a fileauditedpermissions_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M575"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M575"/>
   <xsl:template match="@*|node()" priority="-2" mode="M575">
      <xsl:apply-templates select="@*|node()" mode="M575"/>
   </xsl:template>

   <!--PATTERN fapobjpath-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_object/win-def:path" priority="4000"
                 mode="M576">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileauditedpermissions_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M576"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M576"/>
   <xsl:template match="@*|node()" priority="-2" mode="M576">
      <xsl:apply-templates select="@*|node()" mode="M576"/>
   </xsl:template>

   <!--PATTERN fapobjfilename-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_object/win-def:filename" priority="4000"
                 mode="M577">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M577"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M577"/>
   <xsl:template match="@*|node()" priority="-2" mode="M577">
      <xsl:apply-templates select="@*|node()" mode="M577"/>
   </xsl:template>

   <!--PATTERN fapobjtrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_object/win-def:trustee_name"
                 priority="4000"
                 mode="M578">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a fileauditedpermissions_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M578"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M578"/>
   <xsl:template match="@*|node()" priority="-2" mode="M578">
      <xsl:apply-templates select="@*|node()" mode="M578"/>
   </xsl:template>

   <!--PATTERN fapstepath-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:path" priority="4000"
                 mode="M579">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M579"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M579"/>
   <xsl:template match="@*|node()" priority="-2" mode="M579">
      <xsl:apply-templates select="@*|node()" mode="M579"/>
   </xsl:template>

   <!--PATTERN fapstefilename-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:filename" priority="4000"
                 mode="M580">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M580"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M580"/>
   <xsl:template match="@*|node()" priority="-2" mode="M580">
      <xsl:apply-templates select="@*|node()" mode="M580"/>
   </xsl:template>

   <!--PATTERN fapstetrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:trustee_name"
                 priority="4000"
                 mode="M581">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M581"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M581"/>
   <xsl:template match="@*|node()" priority="-2" mode="M581">
      <xsl:apply-templates select="@*|node()" mode="M581"/>
   </xsl:template>

   <!--PATTERN fapstestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:standard_delete"
                 priority="4000"
                 mode="M582">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M582"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M582"/>
   <xsl:template match="@*|node()" priority="-2" mode="M582">
      <xsl:apply-templates select="@*|node()" mode="M582"/>
   </xsl:template>

   <!--PATTERN fapstestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M583">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M583"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M583"/>
   <xsl:template match="@*|node()" priority="-2" mode="M583">
      <xsl:apply-templates select="@*|node()" mode="M583"/>
   </xsl:template>

   <!--PATTERN fapstestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M584">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M584"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M584"/>
   <xsl:template match="@*|node()" priority="-2" mode="M584">
      <xsl:apply-templates select="@*|node()" mode="M584"/>
   </xsl:template>

   <!--PATTERN fapstestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M585">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M585"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M585"/>
   <xsl:template match="@*|node()" priority="-2" mode="M585">
      <xsl:apply-templates select="@*|node()" mode="M585"/>
   </xsl:template>

   <!--PATTERN fapstestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M586">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M586"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M586"/>
   <xsl:template match="@*|node()" priority="-2" mode="M586">
      <xsl:apply-templates select="@*|node()" mode="M586"/>
   </xsl:template>

   <!--PATTERN fapsteaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:access_system_security"
                 priority="4000"
                 mode="M587">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M587"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M587"/>
   <xsl:template match="@*|node()" priority="-2" mode="M587">
      <xsl:apply-templates select="@*|node()" mode="M587"/>
   </xsl:template>

   <!--PATTERN fapstegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:generic_read"
                 priority="4000"
                 mode="M588">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M588"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M588"/>
   <xsl:template match="@*|node()" priority="-2" mode="M588">
      <xsl:apply-templates select="@*|node()" mode="M588"/>
   </xsl:template>

   <!--PATTERN fapstegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:generic_write"
                 priority="4000"
                 mode="M589">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M589"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M589"/>
   <xsl:template match="@*|node()" priority="-2" mode="M589">
      <xsl:apply-templates select="@*|node()" mode="M589"/>
   </xsl:template>

   <!--PATTERN fapstegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:generic_execute"
                 priority="4000"
                 mode="M590">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M590"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M590"/>
   <xsl:template match="@*|node()" priority="-2" mode="M590">
      <xsl:apply-templates select="@*|node()" mode="M590"/>
   </xsl:template>

   <!--PATTERN fapstegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:generic_all"
                 priority="4000"
                 mode="M591">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M591"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M591"/>
   <xsl:template match="@*|node()" priority="-2" mode="M591">
      <xsl:apply-templates select="@*|node()" mode="M591"/>
   </xsl:template>

   <!--PATTERN fapstefile_read_data-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_read_data"
                 priority="4000"
                 mode="M592">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_data entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M592"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M592"/>
   <xsl:template match="@*|node()" priority="-2" mode="M592">
      <xsl:apply-templates select="@*|node()" mode="M592"/>
   </xsl:template>

   <!--PATTERN fapstefile_write_data-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_write_data"
                 priority="4000"
                 mode="M593">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_data entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M593"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M593"/>
   <xsl:template match="@*|node()" priority="-2" mode="M593">
      <xsl:apply-templates select="@*|node()" mode="M593"/>
   </xsl:template>

   <!--PATTERN fapstefile_append_data-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_append_data"
                 priority="4000"
                 mode="M594">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_append_data entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M594"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M594"/>
   <xsl:template match="@*|node()" priority="-2" mode="M594">
      <xsl:apply-templates select="@*|node()" mode="M594"/>
   </xsl:template>

   <!--PATTERN fapstefile_read_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_read_ea"
                 priority="4000"
                 mode="M595">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_ea entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M595"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M595"/>
   <xsl:template match="@*|node()" priority="-2" mode="M595">
      <xsl:apply-templates select="@*|node()" mode="M595"/>
   </xsl:template>

   <!--PATTERN fapstefile_write_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_write_ea"
                 priority="4000"
                 mode="M596">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_ea entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M596"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M596"/>
   <xsl:template match="@*|node()" priority="-2" mode="M596">
      <xsl:apply-templates select="@*|node()" mode="M596"/>
   </xsl:template>

   <!--PATTERN fapstefile_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_execute"
                 priority="4000"
                 mode="M597">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_execute entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M597"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M597"/>
   <xsl:template match="@*|node()" priority="-2" mode="M597">
      <xsl:apply-templates select="@*|node()" mode="M597"/>
   </xsl:template>

   <!--PATTERN fapstefile_delete_child-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_delete_child"
                 priority="4000"
                 mode="M598">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_delete_child entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M598"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M598"/>
   <xsl:template match="@*|node()" priority="-2" mode="M598">
      <xsl:apply-templates select="@*|node()" mode="M598"/>
   </xsl:template>

   <!--PATTERN fapstefile_read_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_read_attributes"
                 priority="4000"
                 mode="M599">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_attributes entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M599"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M599"/>
   <xsl:template match="@*|node()" priority="-2" mode="M599">
      <xsl:apply-templates select="@*|node()" mode="M599"/>
   </xsl:template>

   <!--PATTERN fapstefile_write_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileauditedpermissions_state/win-def:file_write_attributes"
                 priority="4000"
                 mode="M600">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_attributes entity of a fileauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M600"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M600"/>
   <xsl:template match="@*|node()" priority="-2" mode="M600">
      <xsl:apply-templates select="@*|node()" mode="M600"/>
   </xsl:template>

   <!--PATTERN fer53tst-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_test/win-def:object" priority="4000"
                 mode="M601">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights53_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a fileeffectiverights53_test must reference a fileeffectiverights53_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M601"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_test/win-def:state" priority="3999"
                 mode="M601">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileeffectiverights53_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a fileeffectiverights53_test must reference a fileeffectiverights53_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M601"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M601"/>
   <xsl:template match="@*|node()" priority="-2" mode="M601">
      <xsl:apply-templates select="@*|node()" mode="M601"/>
   </xsl:template>

   <!--PATTERN fer53objpath-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_object/win-def:path" priority="4000"
                 mode="M602">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileeffectiverights53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M602"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M602"/>
   <xsl:template match="@*|node()" priority="-2" mode="M602">
      <xsl:apply-templates select="@*|node()" mode="M602"/>
   </xsl:template>

   <!--PATTERN fer53objfilename-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_object/win-def:filename" priority="4000"
                 mode="M603">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileeffectiverights53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M603"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M603"/>
   <xsl:template match="@*|node()" priority="-2" mode="M603">
      <xsl:apply-templates select="@*|node()" mode="M603"/>
   </xsl:template>

   <!--PATTERN fer53objtrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_object/win-def:trustee_sid"
                 priority="4000"
                 mode="M604">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a fileeffectiverights53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M604"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M604"/>
   <xsl:template match="@*|node()" priority="-2" mode="M604">
      <xsl:apply-templates select="@*|node()" mode="M604"/>
   </xsl:template>

   <!--PATTERN fer53stepath-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:path" priority="4000"
                 mode="M605">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileeffectiverights53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M605"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M605"/>
   <xsl:template match="@*|node()" priority="-2" mode="M605">
      <xsl:apply-templates select="@*|node()" mode="M605"/>
   </xsl:template>

   <!--PATTERN fer53stefilename-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:filename" priority="4000"
                 mode="M606">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileeffectiverights53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M606"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M606"/>
   <xsl:template match="@*|node()" priority="-2" mode="M606">
      <xsl:apply-templates select="@*|node()" mode="M606"/>
   </xsl:template>

   <!--PATTERN fer53stetrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:trustee_sid" priority="4000"
                 mode="M607">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a fileeffectiverights53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M607"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M607"/>
   <xsl:template match="@*|node()" priority="-2" mode="M607">
      <xsl:apply-templates select="@*|node()" mode="M607"/>
   </xsl:template>

   <!--PATTERN fer53stestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:standard_delete"
                 priority="4000"
                 mode="M608">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M608"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M608"/>
   <xsl:template match="@*|node()" priority="-2" mode="M608">
      <xsl:apply-templates select="@*|node()" mode="M608"/>
   </xsl:template>

   <!--PATTERN fer53stestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M609">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M609"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M609"/>
   <xsl:template match="@*|node()" priority="-2" mode="M609">
      <xsl:apply-templates select="@*|node()" mode="M609"/>
   </xsl:template>

   <!--PATTERN fer53stestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M610">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M610"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M610"/>
   <xsl:template match="@*|node()" priority="-2" mode="M610">
      <xsl:apply-templates select="@*|node()" mode="M610"/>
   </xsl:template>

   <!--PATTERN fer53stestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M611">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M611"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M611"/>
   <xsl:template match="@*|node()" priority="-2" mode="M611">
      <xsl:apply-templates select="@*|node()" mode="M611"/>
   </xsl:template>

   <!--PATTERN fer53stestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M612">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M612"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M612"/>
   <xsl:template match="@*|node()" priority="-2" mode="M612">
      <xsl:apply-templates select="@*|node()" mode="M612"/>
   </xsl:template>

   <!--PATTERN fer53steaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:access_system_security"
                 priority="4000"
                 mode="M613">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M613"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M613"/>
   <xsl:template match="@*|node()" priority="-2" mode="M613">
      <xsl:apply-templates select="@*|node()" mode="M613"/>
   </xsl:template>

   <!--PATTERN fer53stegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:generic_read"
                 priority="4000"
                 mode="M614">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M614"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M614"/>
   <xsl:template match="@*|node()" priority="-2" mode="M614">
      <xsl:apply-templates select="@*|node()" mode="M614"/>
   </xsl:template>

   <!--PATTERN fer53stegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:generic_write"
                 priority="4000"
                 mode="M615">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M615"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M615"/>
   <xsl:template match="@*|node()" priority="-2" mode="M615">
      <xsl:apply-templates select="@*|node()" mode="M615"/>
   </xsl:template>

   <!--PATTERN fer53stegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:generic_execute"
                 priority="4000"
                 mode="M616">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M616"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M616"/>
   <xsl:template match="@*|node()" priority="-2" mode="M616">
      <xsl:apply-templates select="@*|node()" mode="M616"/>
   </xsl:template>

   <!--PATTERN fer53stegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:generic_all" priority="4000"
                 mode="M617">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M617"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M617"/>
   <xsl:template match="@*|node()" priority="-2" mode="M617">
      <xsl:apply-templates select="@*|node()" mode="M617"/>
   </xsl:template>

   <!--PATTERN fer53stefile_read_data-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_read_data"
                 priority="4000"
                 mode="M618">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_data entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M618"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M618"/>
   <xsl:template match="@*|node()" priority="-2" mode="M618">
      <xsl:apply-templates select="@*|node()" mode="M618"/>
   </xsl:template>

   <!--PATTERN fer53stefile_write_data-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_write_data"
                 priority="4000"
                 mode="M619">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_data entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M619"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M619"/>
   <xsl:template match="@*|node()" priority="-2" mode="M619">
      <xsl:apply-templates select="@*|node()" mode="M619"/>
   </xsl:template>

   <!--PATTERN fer53stefile_append_data-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_append_data"
                 priority="4000"
                 mode="M620">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_append_data entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M620"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M620"/>
   <xsl:template match="@*|node()" priority="-2" mode="M620">
      <xsl:apply-templates select="@*|node()" mode="M620"/>
   </xsl:template>

   <!--PATTERN fer53stefile_read_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_read_ea"
                 priority="4000"
                 mode="M621">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_ea entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M621"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M621"/>
   <xsl:template match="@*|node()" priority="-2" mode="M621">
      <xsl:apply-templates select="@*|node()" mode="M621"/>
   </xsl:template>

   <!--PATTERN fer53stefile_write_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_write_ea"
                 priority="4000"
                 mode="M622">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_ea entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M622"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M622"/>
   <xsl:template match="@*|node()" priority="-2" mode="M622">
      <xsl:apply-templates select="@*|node()" mode="M622"/>
   </xsl:template>

   <!--PATTERN fer53stefile_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_execute"
                 priority="4000"
                 mode="M623">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_execute entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M623"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M623"/>
   <xsl:template match="@*|node()" priority="-2" mode="M623">
      <xsl:apply-templates select="@*|node()" mode="M623"/>
   </xsl:template>

   <!--PATTERN fer53stefile_delete_child-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_delete_child"
                 priority="4000"
                 mode="M624">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_delete_child entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M624"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M624"/>
   <xsl:template match="@*|node()" priority="-2" mode="M624">
      <xsl:apply-templates select="@*|node()" mode="M624"/>
   </xsl:template>

   <!--PATTERN fer53stefile_read_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_read_attributes"
                 priority="4000"
                 mode="M625">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_attributes entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M625"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M625"/>
   <xsl:template match="@*|node()" priority="-2" mode="M625">
      <xsl:apply-templates select="@*|node()" mode="M625"/>
   </xsl:template>

   <!--PATTERN fer53stefile_write_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights53_state/win-def:file_write_attributes"
                 priority="4000"
                 mode="M626">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_attributes entity of a fileeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M626"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M626"/>
   <xsl:template match="@*|node()" priority="-2" mode="M626">
      <xsl:apply-templates select="@*|node()" mode="M626"/>
   </xsl:template>

   <!--PATTERN fertst-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_test/win-def:object" priority="4000"
                 mode="M627">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:fileeffectiverights_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a fileeffectiverights_test must reference a fileeffectiverights_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M627"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:fileeffectiverights_test/win-def:state" priority="3999"
                 mode="M627">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:fileeffectiverights_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a fileeffectiverights_test must reference a fileeffectiverights_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M627"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M627"/>
   <xsl:template match="@*|node()" priority="-2" mode="M627">
      <xsl:apply-templates select="@*|node()" mode="M627"/>
   </xsl:template>

   <!--PATTERN fefobjpath-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_object/win-def:path" priority="4000"
                 mode="M628">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileeffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M628"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M628"/>
   <xsl:template match="@*|node()" priority="-2" mode="M628">
      <xsl:apply-templates select="@*|node()" mode="M628"/>
   </xsl:template>

   <!--PATTERN fefobjfilename-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_object/win-def:filename" priority="4000"
                 mode="M629">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileeffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M629"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M629"/>
   <xsl:template match="@*|node()" priority="-2" mode="M629">
      <xsl:apply-templates select="@*|node()" mode="M629"/>
   </xsl:template>

   <!--PATTERN fefobjtrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_object/win-def:trustee_name" priority="4000"
                 mode="M630">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a fileeffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M630"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M630"/>
   <xsl:template match="@*|node()" priority="-2" mode="M630">
      <xsl:apply-templates select="@*|node()" mode="M630"/>
   </xsl:template>

   <!--PATTERN ferstepath-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:path" priority="4000"
                 mode="M631">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the path entity of a fileeffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M631"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M631"/>
   <xsl:template match="@*|node()" priority="-2" mode="M631">
      <xsl:apply-templates select="@*|node()" mode="M631"/>
   </xsl:template>

   <!--PATTERN ferstefilename-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:filename" priority="4000"
                 mode="M632">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the filename entity of a fileeffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="if (not(@operation) or @operation='equals' or @operation='not equal') then (not(contains(.,'\') or contains(.,'/') or contains(.,':') or contains(.,'*') or contains(.,'?') or contains(.,'&gt;') or contains(.,'|'))) else (1=1)"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - filename entity can not contain the characters / \ : * ? &gt; | <xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M632"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M632"/>
   <xsl:template match="@*|node()" priority="-2" mode="M632">
      <xsl:apply-templates select="@*|node()" mode="M632"/>
   </xsl:template>

   <!--PATTERN ferstetrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:trustee_name" priority="4000"
                 mode="M633">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a fileeffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M633"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M633"/>
   <xsl:template match="@*|node()" priority="-2" mode="M633">
      <xsl:apply-templates select="@*|node()" mode="M633"/>
   </xsl:template>

   <!--PATTERN ferstestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:standard_delete"
                 priority="4000"
                 mode="M634">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M634"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M634"/>
   <xsl:template match="@*|node()" priority="-2" mode="M634">
      <xsl:apply-templates select="@*|node()" mode="M634"/>
   </xsl:template>

   <!--PATTERN ferstestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M635">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M635"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M635"/>
   <xsl:template match="@*|node()" priority="-2" mode="M635">
      <xsl:apply-templates select="@*|node()" mode="M635"/>
   </xsl:template>

   <!--PATTERN ferstestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M636">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M636"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M636"/>
   <xsl:template match="@*|node()" priority="-2" mode="M636">
      <xsl:apply-templates select="@*|node()" mode="M636"/>
   </xsl:template>

   <!--PATTERN ferstestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M637">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M637"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M637"/>
   <xsl:template match="@*|node()" priority="-2" mode="M637">
      <xsl:apply-templates select="@*|node()" mode="M637"/>
   </xsl:template>

   <!--PATTERN ferstestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M638">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M638"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M638"/>
   <xsl:template match="@*|node()" priority="-2" mode="M638">
      <xsl:apply-templates select="@*|node()" mode="M638"/>
   </xsl:template>

   <!--PATTERN fersteaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:access_system_security"
                 priority="4000"
                 mode="M639">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M639"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M639"/>
   <xsl:template match="@*|node()" priority="-2" mode="M639">
      <xsl:apply-templates select="@*|node()" mode="M639"/>
   </xsl:template>

   <!--PATTERN ferstegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:generic_read" priority="4000"
                 mode="M640">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M640"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M640"/>
   <xsl:template match="@*|node()" priority="-2" mode="M640">
      <xsl:apply-templates select="@*|node()" mode="M640"/>
   </xsl:template>

   <!--PATTERN ferstegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:generic_write" priority="4000"
                 mode="M641">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M641"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M641"/>
   <xsl:template match="@*|node()" priority="-2" mode="M641">
      <xsl:apply-templates select="@*|node()" mode="M641"/>
   </xsl:template>

   <!--PATTERN ferstegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:generic_execute"
                 priority="4000"
                 mode="M642">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M642"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M642"/>
   <xsl:template match="@*|node()" priority="-2" mode="M642">
      <xsl:apply-templates select="@*|node()" mode="M642"/>
   </xsl:template>

   <!--PATTERN ferstegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:generic_all" priority="4000"
                 mode="M643">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M643"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M643"/>
   <xsl:template match="@*|node()" priority="-2" mode="M643">
      <xsl:apply-templates select="@*|node()" mode="M643"/>
   </xsl:template>

   <!--PATTERN ferstefile_read_data-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_read_data"
                 priority="4000"
                 mode="M644">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_data entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M644"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M644"/>
   <xsl:template match="@*|node()" priority="-2" mode="M644">
      <xsl:apply-templates select="@*|node()" mode="M644"/>
   </xsl:template>

   <!--PATTERN ferstefile_write_data-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_write_data"
                 priority="4000"
                 mode="M645">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_data entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M645"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M645"/>
   <xsl:template match="@*|node()" priority="-2" mode="M645">
      <xsl:apply-templates select="@*|node()" mode="M645"/>
   </xsl:template>

   <!--PATTERN ferstefile_append_data-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_append_data"
                 priority="4000"
                 mode="M646">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_append_data entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M646"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M646"/>
   <xsl:template match="@*|node()" priority="-2" mode="M646">
      <xsl:apply-templates select="@*|node()" mode="M646"/>
   </xsl:template>

   <!--PATTERN ferstefile_read_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_read_ea" priority="4000"
                 mode="M647">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_ea entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M647"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M647"/>
   <xsl:template match="@*|node()" priority="-2" mode="M647">
      <xsl:apply-templates select="@*|node()" mode="M647"/>
   </xsl:template>

   <!--PATTERN ferstefile_write_ea-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_write_ea" priority="4000"
                 mode="M648">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_ea entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M648"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M648"/>
   <xsl:template match="@*|node()" priority="-2" mode="M648">
      <xsl:apply-templates select="@*|node()" mode="M648"/>
   </xsl:template>

   <!--PATTERN ferstefile_execute-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_execute" priority="4000"
                 mode="M649">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_execute entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M649"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M649"/>
   <xsl:template match="@*|node()" priority="-2" mode="M649">
      <xsl:apply-templates select="@*|node()" mode="M649"/>
   </xsl:template>

   <!--PATTERN ferstefile_delete_child-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_delete_child"
                 priority="4000"
                 mode="M650">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_delete_child entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M650"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M650"/>
   <xsl:template match="@*|node()" priority="-2" mode="M650">
      <xsl:apply-templates select="@*|node()" mode="M650"/>
   </xsl:template>

   <!--PATTERN ferstefile_read_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_read_attributes"
                 priority="4000"
                 mode="M651">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_attributes entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M651"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M651"/>
   <xsl:template match="@*|node()" priority="-2" mode="M651">
      <xsl:apply-templates select="@*|node()" mode="M651"/>
   </xsl:template>

   <!--PATTERN ferstefile_write_attributes-->


	<!--RULE -->
<xsl:template match="win-def:fileeffectiverights_state/win-def:file_write_attributes"
                 priority="4000"
                 mode="M652">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_write_attributes entity of a fileeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M652"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M652"/>
   <xsl:template match="@*|node()" priority="-2" mode="M652">
      <xsl:apply-templates select="@*|node()" mode="M652"/>
   </xsl:template>

   <!--PATTERN grouptst-->


	<!--RULE -->
<xsl:template match="win-def:group_test/win-def:object" priority="4000" mode="M653">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:group_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a group_test must reference a group_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M653"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:group_test/win-def:state" priority="3999" mode="M653">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:group_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a group_test must reference a group_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M653"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M653"/>
   <xsl:template match="@*|node()" priority="-2" mode="M653">
      <xsl:apply-templates select="@*|node()" mode="M653"/>
   </xsl:template>

   <!--PATTERN groupobjgroup-->


	<!--RULE -->
<xsl:template match="win-def:group_object/win-def:group" priority="4000" mode="M654">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group entity of a group_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M654"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M654"/>
   <xsl:template match="@*|node()" priority="-2" mode="M654">
      <xsl:apply-templates select="@*|node()" mode="M654"/>
   </xsl:template>

   <!--PATTERN groupstegroup-->


	<!--RULE -->
<xsl:template match="win-def:group_state/win-def:group" priority="4000" mode="M655">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group entity of a group_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M655"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M655"/>
   <xsl:template match="@*|node()" priority="-2" mode="M655">
      <xsl:apply-templates select="@*|node()" mode="M655"/>
   </xsl:template>

   <!--PATTERN groupsteuser-->


	<!--RULE -->
<xsl:template match="win-def:group_state/win-def:user" priority="4000" mode="M656">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user entity of a group_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M656"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M656"/>
   <xsl:template match="@*|node()" priority="-2" mode="M656">
      <xsl:apply-templates select="@*|node()" mode="M656"/>
   </xsl:template>

   <!--PATTERN groupsidtst-->


	<!--RULE -->
<xsl:template match="win-def:group_sid_test/win-def:object" priority="4000" mode="M657">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:group_sid_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a group_sid_test must reference a group_sid_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M657"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:group_sid_test/win-def:state" priority="3999" mode="M657">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:group_sid_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a group_sid_test must reference a group_sid_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M657"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M657"/>
   <xsl:template match="@*|node()" priority="-2" mode="M657">
      <xsl:apply-templates select="@*|node()" mode="M657"/>
   </xsl:template>

   <!--PATTERN groupsidobjgroup-->


	<!--RULE -->
<xsl:template match="win-def:group_sid_object/win-def:group_sid" priority="4000" mode="M658">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group_sid entity of a group_sid_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M658"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M658"/>
   <xsl:template match="@*|node()" priority="-2" mode="M658">
      <xsl:apply-templates select="@*|node()" mode="M658"/>
   </xsl:template>

   <!--PATTERN groupsidstegroup-->


	<!--RULE -->
<xsl:template match="win-def:group_sid_state/win-def:group_sid" priority="4000" mode="M659">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group_sid entity of a group_sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M659"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M659"/>
   <xsl:template match="@*|node()" priority="-2" mode="M659">
      <xsl:apply-templates select="@*|node()" mode="M659"/>
   </xsl:template>

   <!--PATTERN groupsidsteuser-->


	<!--RULE -->
<xsl:template match="win-def:group_sid_state/win-def:user_sid" priority="4000" mode="M660">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_sid entity of a group_sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M660"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M660"/>
   <xsl:template match="@*|node()" priority="-2" mode="M660">
      <xsl:apply-templates select="@*|node()" mode="M660"/>
   </xsl:template>

   <!--PATTERN wininterfacetst-->


	<!--RULE -->
<xsl:template match="win-def:interface_test/win-def:object" priority="4000" mode="M661">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:interface_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of an interface_test must reference an interface_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M661"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:interface_test/win-def:state" priority="3999" mode="M661">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:interface_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of an interface_test must reference an interface_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M661"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M661"/>
   <xsl:template match="@*|node()" priority="-2" mode="M661">
      <xsl:apply-templates select="@*|node()" mode="M661"/>
   </xsl:template>

   <!--PATTERN wininterfaceobjname-->


	<!--RULE -->
<xsl:template match="win-def:interface_object/win-def:name" priority="4000" mode="M662">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a interface_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M662"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M662"/>
   <xsl:template match="@*|node()" priority="-2" mode="M662">
      <xsl:apply-templates select="@*|node()" mode="M662"/>
   </xsl:template>

   <!--PATTERN wininterfacestename-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:name" priority="4000" mode="M663">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M663"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M663"/>
   <xsl:template match="@*|node()" priority="-2" mode="M663">
      <xsl:apply-templates select="@*|node()" mode="M663"/>
   </xsl:template>

   <!--PATTERN wininterfacesteindex-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:index" priority="4000" mode="M664">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the index entity of an interface_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M664"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M664"/>
   <xsl:template match="@*|node()" priority="-2" mode="M664">
      <xsl:apply-templates select="@*|node()" mode="M664"/>
   </xsl:template>

   <!--PATTERN wininterfacestetype-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:type" priority="4000" mode="M665">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the type entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M665"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M665"/>
   <xsl:template match="@*|node()" priority="-2" mode="M665">
      <xsl:apply-templates select="@*|node()" mode="M665"/>
   </xsl:template>

   <!--PATTERN wininterfacestehardware_addr-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:hardware_addr" priority="4000"
                 mode="M666">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hardware_addr entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M666"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M666"/>
   <xsl:template match="@*|node()" priority="-2" mode="M666">
      <xsl:apply-templates select="@*|node()" mode="M666"/>
   </xsl:template>

   <!--PATTERN wininterfacesteinet_addr-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:inet_addr" priority="4000" mode="M667">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the inet_addr entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M667"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M667"/>
   <xsl:template match="@*|node()" priority="-2" mode="M667">
      <xsl:apply-templates select="@*|node()" mode="M667"/>
   </xsl:template>

   <!--PATTERN wininterfacestebroadcast_addr-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:broadcast_addr" priority="4000"
                 mode="M668">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the broadcast_addr entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M668"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M668"/>
   <xsl:template match="@*|node()" priority="-2" mode="M668">
      <xsl:apply-templates select="@*|node()" mode="M668"/>
   </xsl:template>

   <!--PATTERN wininterfacestenetmask-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:netmask" priority="4000" mode="M669">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the netmask entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M669"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M669"/>
   <xsl:template match="@*|node()" priority="-2" mode="M669">
      <xsl:apply-templates select="@*|node()" mode="M669"/>
   </xsl:template>

   <!--PATTERN wininterfacesteaddr_type-->


	<!--RULE -->
<xsl:template match="win-def:interface_state/win-def:addr_type" priority="4000" mode="M670">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the addr_type entity of an interface_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M670"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M670"/>
   <xsl:template match="@*|node()" priority="-2" mode="M670">
      <xsl:apply-templates select="@*|node()" mode="M670"/>
   </xsl:template>

   <!--PATTERN lptst-->


	<!--RULE -->
<xsl:template match="win-def:lockoutpolicy_test/win-def:object" priority="4000" mode="M671">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:lockoutpolicy_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a lockoutpolicy_test must reference a lockoutpolicy_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M671"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:lockoutpolicy_test/win-def:state" priority="3999" mode="M671">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:lockoutpolicy_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a lockoutpolicy_test must reference a lockoutpolicy_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M671"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M671"/>
   <xsl:template match="@*|node()" priority="-2" mode="M671">
      <xsl:apply-templates select="@*|node()" mode="M671"/>
   </xsl:template>

   <!--PATTERN lpsteforce_logoff-->


	<!--RULE -->
<xsl:template match="win-def:lockoutpolicy_state/win-def:force_logoff" priority="4000"
                 mode="M672">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the force_logoff entity of a lockoutpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M672"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M672"/>
   <xsl:template match="@*|node()" priority="-2" mode="M672">
      <xsl:apply-templates select="@*|node()" mode="M672"/>
   </xsl:template>

   <!--PATTERN lpstelockout_duration-->


	<!--RULE -->
<xsl:template match="win-def:lockoutpolicy_state/win-def:lockout_duration" priority="4000"
                 mode="M673">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the lockout_duration entity of a lockoutpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M673"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M673"/>
   <xsl:template match="@*|node()" priority="-2" mode="M673">
      <xsl:apply-templates select="@*|node()" mode="M673"/>
   </xsl:template>

   <!--PATTERN lpstelockout_observation_window-->


	<!--RULE -->
<xsl:template match="win-def:lockoutpolicy_state/win-def:lockout_observation_window"
                 priority="4000"
                 mode="M674">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the lockout_observation_window entity of a lockoutpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M674"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M674"/>
   <xsl:template match="@*|node()" priority="-2" mode="M674">
      <xsl:apply-templates select="@*|node()" mode="M674"/>
   </xsl:template>

   <!--PATTERN lpstelockout_threshold-->


	<!--RULE -->
<xsl:template match="win-def:lockoutpolicy_state/win-def:lockout_threshold" priority="4000"
                 mode="M675">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the lockout_threshold entity of a lockoutpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M675"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M675"/>
   <xsl:template match="@*|node()" priority="-2" mode="M675">
      <xsl:apply-templates select="@*|node()" mode="M675"/>
   </xsl:template>

   <!--PATTERN metabasetst-->


	<!--RULE -->
<xsl:template match="win-def:metabase_test/win-def:object" priority="4000" mode="M676">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:metabase_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a metabase_test must reference a metabase_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M676"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:metabase_test/win-def:state" priority="3999" mode="M676">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:metabase_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a metabase_test must reference a metabase_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M676"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M676"/>
   <xsl:template match="@*|node()" priority="-2" mode="M676">
      <xsl:apply-templates select="@*|node()" mode="M676"/>
   </xsl:template>

   <!--PATTERN metabaseobjkey-->


	<!--RULE -->
<xsl:template match="win-def:metabase_object/win-def:key" priority="4000" mode="M677">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a metabase_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M677"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M677"/>
   <xsl:template match="@*|node()" priority="-2" mode="M677">
      <xsl:apply-templates select="@*|node()" mode="M677"/>
   </xsl:template>

   <!--PATTERN metabaseobjid-->


	<!--RULE -->
<xsl:template match="win-def:metabase_object/win-def:id" priority="4000" mode="M678">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the id entity of a metabase_object should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M678"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M678"/>
   <xsl:template match="@*|node()" priority="-2" mode="M678">
      <xsl:apply-templates select="@*|node()" mode="M678"/>
   </xsl:template>

   <!--PATTERN metabasestekey-->


	<!--RULE -->
<xsl:template match="win-def:metabase_state/win-def:key" priority="4000" mode="M679">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a metabase_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M679"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M679"/>
   <xsl:template match="@*|node()" priority="-2" mode="M679">
      <xsl:apply-templates select="@*|node()" mode="M679"/>
   </xsl:template>

   <!--PATTERN metabasesteid-->


	<!--RULE -->
<xsl:template match="win-def:metabase_state/win-def:id" priority="4000" mode="M680">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the id entity of a metabase_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M680"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M680"/>
   <xsl:template match="@*|node()" priority="-2" mode="M680">
      <xsl:apply-templates select="@*|node()" mode="M680"/>
   </xsl:template>

   <!--PATTERN metabasestename-->


	<!--RULE -->
<xsl:template match="win-def:metabase_state/win-def:name" priority="4000" mode="M681">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a metabase_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M681"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M681"/>
   <xsl:template match="@*|node()" priority="-2" mode="M681">
      <xsl:apply-templates select="@*|node()" mode="M681"/>
   </xsl:template>

   <!--PATTERN metabasesteuser_type-->


	<!--RULE -->
<xsl:template match="win-def:metabase_state/win-def:user_type" priority="4000" mode="M682">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_type entity of a metabase_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M682"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M682"/>
   <xsl:template match="@*|node()" priority="-2" mode="M682">
      <xsl:apply-templates select="@*|node()" mode="M682"/>
   </xsl:template>

   <!--PATTERN metabasestedata_type-->


	<!--RULE -->
<xsl:template match="win-def:metabase_state/win-def:data_type" priority="4000" mode="M683">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the data_type entity of a metabase_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M683"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M683"/>
   <xsl:template match="@*|node()" priority="-2" mode="M683">
      <xsl:apply-templates select="@*|node()" mode="M683"/>
   </xsl:template>

   <!--PATTERN metabasestedata-->


	<!--RULE -->
<xsl:template match="win-def:metabase_state/win-def:data" priority="4000" mode="M684">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M684"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M684"/>
   <xsl:template match="@*|node()" priority="-2" mode="M684">
      <xsl:apply-templates select="@*|node()" mode="M684"/>
   </xsl:template>

   <!--PATTERN pptst-->


	<!--RULE -->
<xsl:template match="win-def:passwordpolicy_test/win-def:object" priority="4000" mode="M685">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:passwordpolicy_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a passwordpolicy_test must reference a passwordpolicy_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M685"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:passwordpolicy_test/win-def:state" priority="3999" mode="M685">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:passwordpolicy_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a passwordpolicy_test must reference a passwordpolicy_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M685"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M685"/>
   <xsl:template match="@*|node()" priority="-2" mode="M685">
      <xsl:apply-templates select="@*|node()" mode="M685"/>
   </xsl:template>

   <!--PATTERN ppstemax_passwd_age-->


	<!--RULE -->
<xsl:template match="win-def:passwordpolicy_state/win-def:max_passwd_age" priority="4000"
                 mode="M686">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the max_passwd_age entity of a passwordpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M686"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M686"/>
   <xsl:template match="@*|node()" priority="-2" mode="M686">
      <xsl:apply-templates select="@*|node()" mode="M686"/>
   </xsl:template>

   <!--PATTERN ppstemin_passwd_age-->


	<!--RULE -->
<xsl:template match="win-def:passwordpolicy_state/win-def:min_passwd_age" priority="4000"
                 mode="M687">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the min_passwd_age entity of a passwordpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M687"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M687"/>
   <xsl:template match="@*|node()" priority="-2" mode="M687">
      <xsl:apply-templates select="@*|node()" mode="M687"/>
   </xsl:template>

   <!--PATTERN ppstemin_passwd_len-->


	<!--RULE -->
<xsl:template match="win-def:passwordpolicy_state/win-def:min_passwd_len" priority="4000"
                 mode="M688">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the min_passwd_len entity of a passwordpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M688"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M688"/>
   <xsl:template match="@*|node()" priority="-2" mode="M688">
      <xsl:apply-templates select="@*|node()" mode="M688"/>
   </xsl:template>

   <!--PATTERN ppstepassword_hist_len-->


	<!--RULE -->
<xsl:template match="win-def:passwordpolicy_state/win-def:password_hist_len" priority="4000"
                 mode="M689">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the password_hist_len entity of a passwordpolicy_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M689"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M689"/>
   <xsl:template match="@*|node()" priority="-2" mode="M689">
      <xsl:apply-templates select="@*|node()" mode="M689"/>
   </xsl:template>

   <!--PATTERN ppstepassword_complexity-->


	<!--RULE -->
<xsl:template match="win-def:passwordpolicy_state/win-def:password_complexity"
                 priority="4000"
                 mode="M690">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the password_complexity entity of an passwordpolicy_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M690"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M690"/>
   <xsl:template match="@*|node()" priority="-2" mode="M690">
      <xsl:apply-templates select="@*|node()" mode="M690"/>
   </xsl:template>

   <!--PATTERN ppstereversible_encryption-->


	<!--RULE -->
<xsl:template match="win-def:passwordpolicy_state/win-def:reversible_encryption"
                 priority="4000"
                 mode="M691">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the reversible_encryption entity of an passwordpolicy_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M691"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M691"/>
   <xsl:template match="@*|node()" priority="-2" mode="M691">
      <xsl:apply-templates select="@*|node()" mode="M691"/>
   </xsl:template>

   <!--PATTERN winporttst-->


	<!--RULE -->
<xsl:template match="win-def:port_test/win-def:object" priority="4000" mode="M692">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:port_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a port_test must reference a port_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M692"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:port_test/win-def:state" priority="3999" mode="M692">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:port_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a port_test must reference a port_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M692"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M692"/>
   <xsl:template match="@*|node()" priority="-2" mode="M692">
      <xsl:apply-templates select="@*|node()" mode="M692"/>
   </xsl:template>

   <!--PATTERN winportobjlocal_address-->


	<!--RULE -->
<xsl:template match="win-def:port_object/win-def:local_address" priority="4000" mode="M693">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_address entity of a port_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M693"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M693"/>
   <xsl:template match="@*|node()" priority="-2" mode="M693">
      <xsl:apply-templates select="@*|node()" mode="M693"/>
   </xsl:template>

   <!--PATTERN winportobjlocal_port-->


	<!--RULE -->
<xsl:template match="win-def:port_object/win-def:local_port" priority="4000" mode="M694">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_port entity of a port_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M694"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M694"/>
   <xsl:template match="@*|node()" priority="-2" mode="M694">
      <xsl:apply-templates select="@*|node()" mode="M694"/>
   </xsl:template>

   <!--PATTERN winportobjprotocol-->


	<!--RULE -->
<xsl:template match="win-def:port_object/win-def:protocol" priority="4000" mode="M695">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of a port_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M695"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M695"/>
   <xsl:template match="@*|node()" priority="-2" mode="M695">
      <xsl:apply-templates select="@*|node()" mode="M695"/>
   </xsl:template>

   <!--PATTERN winportstelocal_address-->


	<!--RULE -->
<xsl:template match="win-def:port_state/win-def:local_address" priority="4000" mode="M696">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_address entity of a port_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M696"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M696"/>
   <xsl:template match="@*|node()" priority="-2" mode="M696">
      <xsl:apply-templates select="@*|node()" mode="M696"/>
   </xsl:template>

   <!--PATTERN winportstelocal_port-->


	<!--RULE -->
<xsl:template match="win-def:port_state/win-def:local_port" priority="4000" mode="M697">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_port entity of a port_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M697"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M697"/>
   <xsl:template match="@*|node()" priority="-2" mode="M697">
      <xsl:apply-templates select="@*|node()" mode="M697"/>
   </xsl:template>

   <!--PATTERN winportsteprotocol-->


	<!--RULE -->
<xsl:template match="win-def:port_state/win-def:protocol" priority="4000" mode="M698">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the protocol entity of a port_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M698"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M698"/>
   <xsl:template match="@*|node()" priority="-2" mode="M698">
      <xsl:apply-templates select="@*|node()" mode="M698"/>
   </xsl:template>

   <!--PATTERN winportstepid-->


	<!--RULE -->
<xsl:template match="win-def:port_state/win-def:pid" priority="4000" mode="M699">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pid entity of a port_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M699"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M699"/>
   <xsl:template match="@*|node()" priority="-2" mode="M699">
      <xsl:apply-templates select="@*|node()" mode="M699"/>
   </xsl:template>

   <!--PATTERN pertst-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_test/win-def:object" priority="4000"
                 mode="M700">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:printereffectiverights_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a printereffectiverights_test must reference a printereffectiverights_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M700"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:printereffectiverights_test/win-def:state" priority="3999"
                 mode="M700">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:printereffectiverights_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a printereffectiverights_test must reference a printereffectiverights_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M700"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M700"/>
   <xsl:template match="@*|node()" priority="-2" mode="M700">
      <xsl:apply-templates select="@*|node()" mode="M700"/>
   </xsl:template>

   <!--PATTERN perobjprinter_name-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_object/win-def:printer_name"
                 priority="4000"
                 mode="M701">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the kprinter_nameey entity of a printereffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M701"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M701"/>
   <xsl:template match="@*|node()" priority="-2" mode="M701">
      <xsl:apply-templates select="@*|node()" mode="M701"/>
   </xsl:template>

   <!--PATTERN perobjtrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_object/win-def:trustee_sid"
                 priority="4000"
                 mode="M702">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a printereffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M702"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M702"/>
   <xsl:template match="@*|node()" priority="-2" mode="M702">
      <xsl:apply-templates select="@*|node()" mode="M702"/>
   </xsl:template>

   <!--PATTERN rersteprinter_name-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:printer_name"
                 priority="4000"
                 mode="M703">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the printer_name entity of a printereffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M703"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M703"/>
   <xsl:template match="@*|node()" priority="-2" mode="M703">
      <xsl:apply-templates select="@*|node()" mode="M703"/>
   </xsl:template>

   <!--PATTERN perstetrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:trustee_sid"
                 priority="4000"
                 mode="M704">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a printereffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M704"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M704"/>
   <xsl:template match="@*|node()" priority="-2" mode="M704">
      <xsl:apply-templates select="@*|node()" mode="M704"/>
   </xsl:template>

   <!--PATTERN perstestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:standard_delete"
                 priority="4000"
                 mode="M705">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M705"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M705"/>
   <xsl:template match="@*|node()" priority="-2" mode="M705">
      <xsl:apply-templates select="@*|node()" mode="M705"/>
   </xsl:template>

   <!--PATTERN perstestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M706">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M706"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M706"/>
   <xsl:template match="@*|node()" priority="-2" mode="M706">
      <xsl:apply-templates select="@*|node()" mode="M706"/>
   </xsl:template>

   <!--PATTERN perstestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M707">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M707"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M707"/>
   <xsl:template match="@*|node()" priority="-2" mode="M707">
      <xsl:apply-templates select="@*|node()" mode="M707"/>
   </xsl:template>

   <!--PATTERN perstestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M708">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M708"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M708"/>
   <xsl:template match="@*|node()" priority="-2" mode="M708">
      <xsl:apply-templates select="@*|node()" mode="M708"/>
   </xsl:template>

   <!--PATTERN perstestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M709">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M709"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M709"/>
   <xsl:template match="@*|node()" priority="-2" mode="M709">
      <xsl:apply-templates select="@*|node()" mode="M709"/>
   </xsl:template>

   <!--PATTERN persteaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:access_system_security"
                 priority="4000"
                 mode="M710">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M710"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M710"/>
   <xsl:template match="@*|node()" priority="-2" mode="M710">
      <xsl:apply-templates select="@*|node()" mode="M710"/>
   </xsl:template>

   <!--PATTERN perstegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:generic_read"
                 priority="4000"
                 mode="M711">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M711"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M711"/>
   <xsl:template match="@*|node()" priority="-2" mode="M711">
      <xsl:apply-templates select="@*|node()" mode="M711"/>
   </xsl:template>

   <!--PATTERN perstegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:generic_write"
                 priority="4000"
                 mode="M712">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M712"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M712"/>
   <xsl:template match="@*|node()" priority="-2" mode="M712">
      <xsl:apply-templates select="@*|node()" mode="M712"/>
   </xsl:template>

   <!--PATTERN perstegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:generic_execute"
                 priority="4000"
                 mode="M713">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M713"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M713"/>
   <xsl:template match="@*|node()" priority="-2" mode="M713">
      <xsl:apply-templates select="@*|node()" mode="M713"/>
   </xsl:template>

   <!--PATTERN perstegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:generic_all"
                 priority="4000"
                 mode="M714">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M714"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M714"/>
   <xsl:template match="@*|node()" priority="-2" mode="M714">
      <xsl:apply-templates select="@*|node()" mode="M714"/>
   </xsl:template>

   <!--PATTERN persteprinter_access_administer-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:printer_access_administer"
                 priority="4000"
                 mode="M715">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the printer_access_administer entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M715"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M715"/>
   <xsl:template match="@*|node()" priority="-2" mode="M715">
      <xsl:apply-templates select="@*|node()" mode="M715"/>
   </xsl:template>

   <!--PATTERN persteprinter_access_use-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:printer_access_use"
                 priority="4000"
                 mode="M716">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the printer_access_use entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M716"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M716"/>
   <xsl:template match="@*|node()" priority="-2" mode="M716">
      <xsl:apply-templates select="@*|node()" mode="M716"/>
   </xsl:template>

   <!--PATTERN perstejob_access_administer-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:job_access_administer"
                 priority="4000"
                 mode="M717">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the job_access_administer entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M717"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M717"/>
   <xsl:template match="@*|node()" priority="-2" mode="M717">
      <xsl:apply-templates select="@*|node()" mode="M717"/>
   </xsl:template>

   <!--PATTERN perstejob_access_read-->


	<!--RULE -->
<xsl:template match="win-def:printereffectiverights_state/win-def:job_access_read"
                 priority="4000"
                 mode="M718">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the job_access_read entity of a printereffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M718"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M718"/>
   <xsl:template match="@*|node()" priority="-2" mode="M718">
      <xsl:apply-templates select="@*|node()" mode="M718"/>
   </xsl:template>

   <!--PATTERN processtst-->


	<!--RULE -->
<xsl:template match="win-def:process_test/win-def:object" priority="4000" mode="M719">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:process_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a process_test must reference a process_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M719"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:process_test/win-def:state" priority="3999" mode="M719">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:process_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a process_test must reference a process_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M719"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M719"/>
   <xsl:template match="@*|node()" priority="-2" mode="M719">
      <xsl:apply-templates select="@*|node()" mode="M719"/>
   </xsl:template>

   <!--PATTERN processobjcommand_line-->


	<!--RULE -->
<xsl:template match="win-def:process_object/win-def:command_line" priority="4000" mode="M720">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the command_line entity of a process_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M720"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M720"/>
   <xsl:template match="@*|node()" priority="-2" mode="M720">
      <xsl:apply-templates select="@*|node()" mode="M720"/>
   </xsl:template>

   <!--PATTERN processstecommand_line-->


	<!--RULE -->
<xsl:template match="win-def:process_state/win-def:command_line" priority="4000" mode="M721">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the command_line entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M721"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M721"/>
   <xsl:template match="@*|node()" priority="-2" mode="M721">
      <xsl:apply-templates select="@*|node()" mode="M721"/>
   </xsl:template>

   <!--PATTERN processstepid-->


	<!--RULE -->
<xsl:template match="win-def:port_state/win-def:pid" priority="4000" mode="M722">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the pid entity of a port_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M722"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M722"/>
   <xsl:template match="@*|node()" priority="-2" mode="M722">
      <xsl:apply-templates select="@*|node()" mode="M722"/>
   </xsl:template>

   <!--PATTERN processsteppid-->


	<!--RULE -->
<xsl:template match="win-def:port_state/win-def:ppid" priority="4000" mode="M723">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the ppid entity of a port_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M723"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M723"/>
   <xsl:template match="@*|node()" priority="-2" mode="M723">
      <xsl:apply-templates select="@*|node()" mode="M723"/>
   </xsl:template>

   <!--PATTERN processstepriority-->


	<!--RULE -->
<xsl:template match="win-def:process_state/win-def:priority" priority="4000" mode="M724">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the priority entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M724"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M724"/>
   <xsl:template match="@*|node()" priority="-2" mode="M724">
      <xsl:apply-templates select="@*|node()" mode="M724"/>
   </xsl:template>

   <!--PATTERN processsteimage_path-->


	<!--RULE -->
<xsl:template match="win-def:process_state/win-def:image_path" priority="4000" mode="M725">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the image_path entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M725"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M725"/>
   <xsl:template match="@*|node()" priority="-2" mode="M725">
      <xsl:apply-templates select="@*|node()" mode="M725"/>
   </xsl:template>

   <!--PATTERN processstecurrent_dir-->


	<!--RULE -->
<xsl:template match="win-def:process_state/win-def:current_dir" priority="4000" mode="M726">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the current_dir entity of a process_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M726"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M726"/>
   <xsl:template match="@*|node()" priority="-2" mode="M726">
      <xsl:apply-templates select="@*|node()" mode="M726"/>
   </xsl:template>

   <!--PATTERN regtst-->


	<!--RULE -->
<xsl:template match="win-def:registry_test/win-def:object" priority="4000" mode="M727">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:registry_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a registry_test must reference a registry_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M727"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:registry_test/win-def:state" priority="3999" mode="M727">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:registry_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a registry_test must reference a registry_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M727"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M727"/>
   <xsl:template match="@*|node()" priority="-2" mode="M727">
      <xsl:apply-templates select="@*|node()" mode="M727"/>
   </xsl:template>

   <!--PATTERN regobjhive-->


	<!--RULE -->
<xsl:template match="win-def:registry_object/win-def:hive" priority="4000" mode="M728">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a registry_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M728"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M728"/>
   <xsl:template match="@*|node()" priority="-2" mode="M728">
      <xsl:apply-templates select="@*|node()" mode="M728"/>
   </xsl:template>

   <!--PATTERN regobjkey-->


	<!--RULE -->
<xsl:template match="win-def:registry_object/win-def:key" priority="4000" mode="M729">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a registry_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>

		    <!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@xsi:nil='true') or ../win-def:name/@xsi:nil='true'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - name entity must be nil when key is nil<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M729"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M729"/>
   <xsl:template match="@*|node()" priority="-2" mode="M729">
      <xsl:apply-templates select="@*|node()" mode="M729"/>
   </xsl:template>

   <!--PATTERN regobjname-->


	<!--RULE -->
<xsl:template match="win-def:registry_object/win-def:name" priority="4000" mode="M730">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a registry_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M730"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M730"/>
   <xsl:template match="@*|node()" priority="-2" mode="M730">
      <xsl:apply-templates select="@*|node()" mode="M730"/>
   </xsl:template>

   <!--PATTERN regstehive-->


	<!--RULE -->
<xsl:template match="win-def:registry_state/win-def:hive" priority="4000" mode="M731">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a registry_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M731"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M731"/>
   <xsl:template match="@*|node()" priority="-2" mode="M731">
      <xsl:apply-templates select="@*|node()" mode="M731"/>
   </xsl:template>

   <!--PATTERN regstekey-->


	<!--RULE -->
<xsl:template match="win-def:registry_state/win-def:key" priority="4000" mode="M732">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a registry_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M732"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M732"/>
   <xsl:template match="@*|node()" priority="-2" mode="M732">
      <xsl:apply-templates select="@*|node()" mode="M732"/>
   </xsl:template>

   <!--PATTERN regstename-->


	<!--RULE -->
<xsl:template match="win-def:registry_state/win-def:name" priority="4000" mode="M733">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a registry_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M733"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M733"/>
   <xsl:template match="@*|node()" priority="-2" mode="M733">
      <xsl:apply-templates select="@*|node()" mode="M733"/>
   </xsl:template>

   <!--PATTERN regstetype-->


	<!--RULE -->
<xsl:template match="win-def:registry_state/win-def:type" priority="4000" mode="M734">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the type entity of a registry_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M734"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M734"/>
   <xsl:template match="@*|node()" priority="-2" mode="M734">
      <xsl:apply-templates select="@*|node()" mode="M734"/>
   </xsl:template>

   <!--PATTERN regstevalue-->


	<!--RULE -->
<xsl:template match="win-def:registry_state/win-def:value" priority="4000" mode="M735">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M735"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M735"/>
   <xsl:template match="@*|node()" priority="-2" mode="M735">
      <xsl:apply-templates select="@*|node()" mode="M735"/>
   </xsl:template>

   <!--PATTERN rap53tst-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_test/win-def:object" priority="4000"
                 mode="M736">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions53_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a regkeyauditedpermissions53_test must reference a regkeyauditedpermissions53_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M736"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_test/win-def:state" priority="3999"
                 mode="M736">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyauditedpermissions53_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a regkeyauditedpermissions53_test must reference a regkeyauditedpermissions53_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M736"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M736"/>
   <xsl:template match="@*|node()" priority="-2" mode="M736">
      <xsl:apply-templates select="@*|node()" mode="M736"/>
   </xsl:template>

   <!--PATTERN rap53objhive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_object/win-def:hive" priority="4000"
                 mode="M737">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M737"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M737"/>
   <xsl:template match="@*|node()" priority="-2" mode="M737">
      <xsl:apply-templates select="@*|node()" mode="M737"/>
   </xsl:template>

   <!--PATTERN rap53objkey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_object/win-def:key" priority="4000"
                 mode="M738">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M738"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M738"/>
   <xsl:template match="@*|node()" priority="-2" mode="M738">
      <xsl:apply-templates select="@*|node()" mode="M738"/>
   </xsl:template>

   <!--PATTERN rap53objtrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_object/win-def:trustee_sid"
                 priority="4000"
                 mode="M739">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a regkeyauditedpermissions53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M739"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M739"/>
   <xsl:template match="@*|node()" priority="-2" mode="M739">
      <xsl:apply-templates select="@*|node()" mode="M739"/>
   </xsl:template>

   <!--PATTERN rap53stehive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:hive" priority="4000"
                 mode="M740">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M740"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M740"/>
   <xsl:template match="@*|node()" priority="-2" mode="M740">
      <xsl:apply-templates select="@*|node()" mode="M740"/>
   </xsl:template>

   <!--PATTERN rap53stekey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key" priority="4000"
                 mode="M741">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M741"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M741"/>
   <xsl:template match="@*|node()" priority="-2" mode="M741">
      <xsl:apply-templates select="@*|node()" mode="M741"/>
   </xsl:template>

   <!--PATTERN rap53stetrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:trustee_sid"
                 priority="4000"
                 mode="M742">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M742"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M742"/>
   <xsl:template match="@*|node()" priority="-2" mode="M742">
      <xsl:apply-templates select="@*|node()" mode="M742"/>
   </xsl:template>

   <!--PATTERN rap53stestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_delete"
                 priority="4000"
                 mode="M743">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M743"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M743"/>
   <xsl:template match="@*|node()" priority="-2" mode="M743">
      <xsl:apply-templates select="@*|node()" mode="M743"/>
   </xsl:template>

   <!--PATTERN rap53stestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M744">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M744"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M744"/>
   <xsl:template match="@*|node()" priority="-2" mode="M744">
      <xsl:apply-templates select="@*|node()" mode="M744"/>
   </xsl:template>

   <!--PATTERN rap53stestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M745">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M745"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M745"/>
   <xsl:template match="@*|node()" priority="-2" mode="M745">
      <xsl:apply-templates select="@*|node()" mode="M745"/>
   </xsl:template>

   <!--PATTERN rap53stestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M746">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M746"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M746"/>
   <xsl:template match="@*|node()" priority="-2" mode="M746">
      <xsl:apply-templates select="@*|node()" mode="M746"/>
   </xsl:template>

   <!--PATTERN rap53stestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M747">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M747"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M747"/>
   <xsl:template match="@*|node()" priority="-2" mode="M747">
      <xsl:apply-templates select="@*|node()" mode="M747"/>
   </xsl:template>

   <!--PATTERN rap53steaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:access_system_security"
                 priority="4000"
                 mode="M748">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M748"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M748"/>
   <xsl:template match="@*|node()" priority="-2" mode="M748">
      <xsl:apply-templates select="@*|node()" mode="M748"/>
   </xsl:template>

   <!--PATTERN rap53stegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_read"
                 priority="4000"
                 mode="M749">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M749"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M749"/>
   <xsl:template match="@*|node()" priority="-2" mode="M749">
      <xsl:apply-templates select="@*|node()" mode="M749"/>
   </xsl:template>

   <!--PATTERN rap53stegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_write"
                 priority="4000"
                 mode="M750">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M750"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M750"/>
   <xsl:template match="@*|node()" priority="-2" mode="M750">
      <xsl:apply-templates select="@*|node()" mode="M750"/>
   </xsl:template>

   <!--PATTERN rap53stegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_execute"
                 priority="4000"
                 mode="M751">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M751"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M751"/>
   <xsl:template match="@*|node()" priority="-2" mode="M751">
      <xsl:apply-templates select="@*|node()" mode="M751"/>
   </xsl:template>

   <!--PATTERN rap53stegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:generic_all"
                 priority="4000"
                 mode="M752">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M752"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M752"/>
   <xsl:template match="@*|node()" priority="-2" mode="M752">
      <xsl:apply-templates select="@*|node()" mode="M752"/>
   </xsl:template>

   <!--PATTERN rap53stekey_query_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_query_value"
                 priority="4000"
                 mode="M753">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_query_value entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M753"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M753"/>
   <xsl:template match="@*|node()" priority="-2" mode="M753">
      <xsl:apply-templates select="@*|node()" mode="M753"/>
   </xsl:template>

   <!--PATTERN rap53stekey_set_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_set_value"
                 priority="4000"
                 mode="M754">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_set_value entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M754"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M754"/>
   <xsl:template match="@*|node()" priority="-2" mode="M754">
      <xsl:apply-templates select="@*|node()" mode="M754"/>
   </xsl:template>

   <!--PATTERN rap53stekey_create_sub_key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_create_sub_key"
                 priority="4000"
                 mode="M755">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M755"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M755"/>
   <xsl:template match="@*|node()" priority="-2" mode="M755">
      <xsl:apply-templates select="@*|node()" mode="M755"/>
   </xsl:template>

   <!--PATTERN rap53stekey_enumerate_sub_keys-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_enumerate_sub_keys"
                 priority="4000"
                 mode="M756">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M756"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M756"/>
   <xsl:template match="@*|node()" priority="-2" mode="M756">
      <xsl:apply-templates select="@*|node()" mode="M756"/>
   </xsl:template>

   <!--PATTERN rap53stekey_notify-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_notify"
                 priority="4000"
                 mode="M757">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_notify entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M757"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M757"/>
   <xsl:template match="@*|node()" priority="-2" mode="M757">
      <xsl:apply-templates select="@*|node()" mode="M757"/>
   </xsl:template>

   <!--PATTERN rap53stekey_create_link-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_create_link"
                 priority="4000"
                 mode="M758">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_link entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M758"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M758"/>
   <xsl:template match="@*|node()" priority="-2" mode="M758">
      <xsl:apply-templates select="@*|node()" mode="M758"/>
   </xsl:template>

   <!--PATTERN rap53stekey_wow64_64key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_wow64_64key"
                 priority="4000"
                 mode="M759">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M759"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M759"/>
   <xsl:template match="@*|node()" priority="-2" mode="M759">
      <xsl:apply-templates select="@*|node()" mode="M759"/>
   </xsl:template>

   <!--PATTERN rap53stekey_wow64_32key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_wow64_32key"
                 priority="4000"
                 mode="M760">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M760"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M760"/>
   <xsl:template match="@*|node()" priority="-2" mode="M760">
      <xsl:apply-templates select="@*|node()" mode="M760"/>
   </xsl:template>

   <!--PATTERN rap53stekey_wow64_res-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions53_state/win-def:key_wow64_res"
                 priority="4000"
                 mode="M761">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyauditedpermissions53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M761"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M761"/>
   <xsl:template match="@*|node()" priority="-2" mode="M761">
      <xsl:apply-templates select="@*|node()" mode="M761"/>
   </xsl:template>

   <!--PATTERN raptst-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_test/win-def:object" priority="4000"
                 mode="M762">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyauditedpermissions_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a regkeyauditedpermissions_test must reference a regkeyauditedpermissions_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M762"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_test/win-def:state" priority="3999"
                 mode="M762">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyauditedpermissions_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a regkeyauditedpermissions_test must reference a regkeyauditedpermissions_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M762"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M762"/>
   <xsl:template match="@*|node()" priority="-2" mode="M762">
      <xsl:apply-templates select="@*|node()" mode="M762"/>
   </xsl:template>

   <!--PATTERN rapobjhive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_object/win-def:hive" priority="4000"
                 mode="M763">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M763"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M763"/>
   <xsl:template match="@*|node()" priority="-2" mode="M763">
      <xsl:apply-templates select="@*|node()" mode="M763"/>
   </xsl:template>

   <!--PATTERN rapobjkey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_object/win-def:key" priority="4000"
                 mode="M764">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M764"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M764"/>
   <xsl:template match="@*|node()" priority="-2" mode="M764">
      <xsl:apply-templates select="@*|node()" mode="M764"/>
   </xsl:template>

   <!--PATTERN rapobjtrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_object/win-def:trustee_name"
                 priority="4000"
                 mode="M765">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a regkeyauditedpermissions_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M765"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M765"/>
   <xsl:template match="@*|node()" priority="-2" mode="M765">
      <xsl:apply-templates select="@*|node()" mode="M765"/>
   </xsl:template>

   <!--PATTERN rapstehive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:hive" priority="4000"
                 mode="M766">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M766"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M766"/>
   <xsl:template match="@*|node()" priority="-2" mode="M766">
      <xsl:apply-templates select="@*|node()" mode="M766"/>
   </xsl:template>

   <!--PATTERN rapstekey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key" priority="4000"
                 mode="M767">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M767"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M767"/>
   <xsl:template match="@*|node()" priority="-2" mode="M767">
      <xsl:apply-templates select="@*|node()" mode="M767"/>
   </xsl:template>

   <!--PATTERN rapstetrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:trustee_name"
                 priority="4000"
                 mode="M768">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M768"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M768"/>
   <xsl:template match="@*|node()" priority="-2" mode="M768">
      <xsl:apply-templates select="@*|node()" mode="M768"/>
   </xsl:template>

   <!--PATTERN rapstestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_delete"
                 priority="4000"
                 mode="M769">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M769"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M769"/>
   <xsl:template match="@*|node()" priority="-2" mode="M769">
      <xsl:apply-templates select="@*|node()" mode="M769"/>
   </xsl:template>

   <!--PATTERN rapstestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M770">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M770"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M770"/>
   <xsl:template match="@*|node()" priority="-2" mode="M770">
      <xsl:apply-templates select="@*|node()" mode="M770"/>
   </xsl:template>

   <!--PATTERN rapstestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M771">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M771"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M771"/>
   <xsl:template match="@*|node()" priority="-2" mode="M771">
      <xsl:apply-templates select="@*|node()" mode="M771"/>
   </xsl:template>

   <!--PATTERN rapstestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M772">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M772"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M772"/>
   <xsl:template match="@*|node()" priority="-2" mode="M772">
      <xsl:apply-templates select="@*|node()" mode="M772"/>
   </xsl:template>

   <!--PATTERN rapstestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M773">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M773"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M773"/>
   <xsl:template match="@*|node()" priority="-2" mode="M773">
      <xsl:apply-templates select="@*|node()" mode="M773"/>
   </xsl:template>

   <!--PATTERN rapsteaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:access_system_security"
                 priority="4000"
                 mode="M774">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M774"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M774"/>
   <xsl:template match="@*|node()" priority="-2" mode="M774">
      <xsl:apply-templates select="@*|node()" mode="M774"/>
   </xsl:template>

   <!--PATTERN rapstegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_read"
                 priority="4000"
                 mode="M775">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M775"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M775"/>
   <xsl:template match="@*|node()" priority="-2" mode="M775">
      <xsl:apply-templates select="@*|node()" mode="M775"/>
   </xsl:template>

   <!--PATTERN rapstegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_write"
                 priority="4000"
                 mode="M776">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M776"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M776"/>
   <xsl:template match="@*|node()" priority="-2" mode="M776">
      <xsl:apply-templates select="@*|node()" mode="M776"/>
   </xsl:template>

   <!--PATTERN rapstegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_execute"
                 priority="4000"
                 mode="M777">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M777"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M777"/>
   <xsl:template match="@*|node()" priority="-2" mode="M777">
      <xsl:apply-templates select="@*|node()" mode="M777"/>
   </xsl:template>

   <!--PATTERN rapstegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:generic_all"
                 priority="4000"
                 mode="M778">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M778"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M778"/>
   <xsl:template match="@*|node()" priority="-2" mode="M778">
      <xsl:apply-templates select="@*|node()" mode="M778"/>
   </xsl:template>

   <!--PATTERN rapstekey_query_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_query_value"
                 priority="4000"
                 mode="M779">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_query_value entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M779"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M779"/>
   <xsl:template match="@*|node()" priority="-2" mode="M779">
      <xsl:apply-templates select="@*|node()" mode="M779"/>
   </xsl:template>

   <!--PATTERN rapstekey_set_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_set_value"
                 priority="4000"
                 mode="M780">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_set_value entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M780"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M780"/>
   <xsl:template match="@*|node()" priority="-2" mode="M780">
      <xsl:apply-templates select="@*|node()" mode="M780"/>
   </xsl:template>

   <!--PATTERN rapstekey_create_sub_key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_create_sub_key"
                 priority="4000"
                 mode="M781">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M781"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M781"/>
   <xsl:template match="@*|node()" priority="-2" mode="M781">
      <xsl:apply-templates select="@*|node()" mode="M781"/>
   </xsl:template>

   <!--PATTERN rapstekey_enumerate_sub_keys-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_enumerate_sub_keys"
                 priority="4000"
                 mode="M782">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M782"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M782"/>
   <xsl:template match="@*|node()" priority="-2" mode="M782">
      <xsl:apply-templates select="@*|node()" mode="M782"/>
   </xsl:template>

   <!--PATTERN rapstekey_notify-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_notify"
                 priority="4000"
                 mode="M783">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_notify entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M783"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M783"/>
   <xsl:template match="@*|node()" priority="-2" mode="M783">
      <xsl:apply-templates select="@*|node()" mode="M783"/>
   </xsl:template>

   <!--PATTERN rapstekey_create_link-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_create_link"
                 priority="4000"
                 mode="M784">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_link entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M784"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M784"/>
   <xsl:template match="@*|node()" priority="-2" mode="M784">
      <xsl:apply-templates select="@*|node()" mode="M784"/>
   </xsl:template>

   <!--PATTERN rapstekey_wow64_64key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_wow64_64key"
                 priority="4000"
                 mode="M785">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M785"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M785"/>
   <xsl:template match="@*|node()" priority="-2" mode="M785">
      <xsl:apply-templates select="@*|node()" mode="M785"/>
   </xsl:template>

   <!--PATTERN rapstekey_wow64_32key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_wow64_32key"
                 priority="4000"
                 mode="M786">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M786"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M786"/>
   <xsl:template match="@*|node()" priority="-2" mode="M786">
      <xsl:apply-templates select="@*|node()" mode="M786"/>
   </xsl:template>

   <!--PATTERN rapstekey_wow64_res-->


	<!--RULE -->
<xsl:template match="win-def:regkeyauditedpermissions_state/win-def:key_wow64_res"
                 priority="4000"
                 mode="M787">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyauditedpermissions_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M787"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M787"/>
   <xsl:template match="@*|node()" priority="-2" mode="M787">
      <xsl:apply-templates select="@*|node()" mode="M787"/>
   </xsl:template>

   <!--PATTERN rer53tst-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_test/win-def:object" priority="4000"
                 mode="M788">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights53_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a regkeyeffectiverights53_test must reference a regkeyeffectiverights53_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M788"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_test/win-def:state" priority="3999"
                 mode="M788">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyeffectiverights53_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a regkeyeffectiverights53_test must reference a regkeyeffectiverights53_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M788"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M788"/>
   <xsl:template match="@*|node()" priority="-2" mode="M788">
      <xsl:apply-templates select="@*|node()" mode="M788"/>
   </xsl:template>

   <!--PATTERN rer53objhive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_object/win-def:hive" priority="4000"
                 mode="M789">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M789"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M789"/>
   <xsl:template match="@*|node()" priority="-2" mode="M789">
      <xsl:apply-templates select="@*|node()" mode="M789"/>
   </xsl:template>

   <!--PATTERN rer53objkey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_object/win-def:key" priority="4000"
                 mode="M790">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M790"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M790"/>
   <xsl:template match="@*|node()" priority="-2" mode="M790">
      <xsl:apply-templates select="@*|node()" mode="M790"/>
   </xsl:template>

   <!--PATTERN rer53objtrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_object/win-def:trustee_sid"
                 priority="4000"
                 mode="M791">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a regkeyeffectiverights53_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M791"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M791"/>
   <xsl:template match="@*|node()" priority="-2" mode="M791">
      <xsl:apply-templates select="@*|node()" mode="M791"/>
   </xsl:template>

   <!--PATTERN rer53stehive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:hive" priority="4000"
                 mode="M792">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M792"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M792"/>
   <xsl:template match="@*|node()" priority="-2" mode="M792">
      <xsl:apply-templates select="@*|node()" mode="M792"/>
   </xsl:template>

   <!--PATTERN rer53stekey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key" priority="4000"
                 mode="M793">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M793"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M793"/>
   <xsl:template match="@*|node()" priority="-2" mode="M793">
      <xsl:apply-templates select="@*|node()" mode="M793"/>
   </xsl:template>

   <!--PATTERN rer53stetrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:trustee_sid"
                 priority="4000"
                 mode="M794">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a regkeyeffectiverights53_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M794"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M794"/>
   <xsl:template match="@*|node()" priority="-2" mode="M794">
      <xsl:apply-templates select="@*|node()" mode="M794"/>
   </xsl:template>

   <!--PATTERN rer53stestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_delete"
                 priority="4000"
                 mode="M795">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M795"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M795"/>
   <xsl:template match="@*|node()" priority="-2" mode="M795">
      <xsl:apply-templates select="@*|node()" mode="M795"/>
   </xsl:template>

   <!--PATTERN rer53stestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M796">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M796"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M796"/>
   <xsl:template match="@*|node()" priority="-2" mode="M796">
      <xsl:apply-templates select="@*|node()" mode="M796"/>
   </xsl:template>

   <!--PATTERN rer53stestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M797">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M797"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M797"/>
   <xsl:template match="@*|node()" priority="-2" mode="M797">
      <xsl:apply-templates select="@*|node()" mode="M797"/>
   </xsl:template>

   <!--PATTERN rer53stestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M798">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M798"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M798"/>
   <xsl:template match="@*|node()" priority="-2" mode="M798">
      <xsl:apply-templates select="@*|node()" mode="M798"/>
   </xsl:template>

   <!--PATTERN rer53stestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M799">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M799"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M799"/>
   <xsl:template match="@*|node()" priority="-2" mode="M799">
      <xsl:apply-templates select="@*|node()" mode="M799"/>
   </xsl:template>

   <!--PATTERN rer53steaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:access_system_security"
                 priority="4000"
                 mode="M800">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M800"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M800"/>
   <xsl:template match="@*|node()" priority="-2" mode="M800">
      <xsl:apply-templates select="@*|node()" mode="M800"/>
   </xsl:template>

   <!--PATTERN rer53stegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_read"
                 priority="4000"
                 mode="M801">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M801"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M801"/>
   <xsl:template match="@*|node()" priority="-2" mode="M801">
      <xsl:apply-templates select="@*|node()" mode="M801"/>
   </xsl:template>

   <!--PATTERN rer53stegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_write"
                 priority="4000"
                 mode="M802">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M802"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M802"/>
   <xsl:template match="@*|node()" priority="-2" mode="M802">
      <xsl:apply-templates select="@*|node()" mode="M802"/>
   </xsl:template>

   <!--PATTERN rer53stegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_execute"
                 priority="4000"
                 mode="M803">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M803"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M803"/>
   <xsl:template match="@*|node()" priority="-2" mode="M803">
      <xsl:apply-templates select="@*|node()" mode="M803"/>
   </xsl:template>

   <!--PATTERN rer53stegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:generic_all"
                 priority="4000"
                 mode="M804">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M804"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M804"/>
   <xsl:template match="@*|node()" priority="-2" mode="M804">
      <xsl:apply-templates select="@*|node()" mode="M804"/>
   </xsl:template>

   <!--PATTERN rer53stekey_query_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_query_value"
                 priority="4000"
                 mode="M805">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_query_value entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M805"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M805"/>
   <xsl:template match="@*|node()" priority="-2" mode="M805">
      <xsl:apply-templates select="@*|node()" mode="M805"/>
   </xsl:template>

   <!--PATTERN rer53stekey_set_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_set_value"
                 priority="4000"
                 mode="M806">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_set_value entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M806"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M806"/>
   <xsl:template match="@*|node()" priority="-2" mode="M806">
      <xsl:apply-templates select="@*|node()" mode="M806"/>
   </xsl:template>

   <!--PATTERN rer53stekey_create_sub_key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_create_sub_key"
                 priority="4000"
                 mode="M807">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M807"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M807"/>
   <xsl:template match="@*|node()" priority="-2" mode="M807">
      <xsl:apply-templates select="@*|node()" mode="M807"/>
   </xsl:template>

   <!--PATTERN rer53stekey_enumerate_sub_keys-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_enumerate_sub_keys"
                 priority="4000"
                 mode="M808">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M808"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M808"/>
   <xsl:template match="@*|node()" priority="-2" mode="M808">
      <xsl:apply-templates select="@*|node()" mode="M808"/>
   </xsl:template>

   <!--PATTERN rer53stekey_notify-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_notify"
                 priority="4000"
                 mode="M809">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_notify entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M809"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M809"/>
   <xsl:template match="@*|node()" priority="-2" mode="M809">
      <xsl:apply-templates select="@*|node()" mode="M809"/>
   </xsl:template>

   <!--PATTERN rer53stekey_create_link-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_create_link"
                 priority="4000"
                 mode="M810">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_link entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M810"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M810"/>
   <xsl:template match="@*|node()" priority="-2" mode="M810">
      <xsl:apply-templates select="@*|node()" mode="M810"/>
   </xsl:template>

   <!--PATTERN rer53stekey_wow64_64key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_wow64_64key"
                 priority="4000"
                 mode="M811">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M811"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M811"/>
   <xsl:template match="@*|node()" priority="-2" mode="M811">
      <xsl:apply-templates select="@*|node()" mode="M811"/>
   </xsl:template>

   <!--PATTERN rer53stekey_wow64_32key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_wow64_32key"
                 priority="4000"
                 mode="M812">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M812"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M812"/>
   <xsl:template match="@*|node()" priority="-2" mode="M812">
      <xsl:apply-templates select="@*|node()" mode="M812"/>
   </xsl:template>

   <!--PATTERN rer53stekey_wow64_res-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights53_state/win-def:key_wow64_res"
                 priority="4000"
                 mode="M813">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyeffectiverights53_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M813"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M813"/>
   <xsl:template match="@*|node()" priority="-2" mode="M813">
      <xsl:apply-templates select="@*|node()" mode="M813"/>
   </xsl:template>

   <!--PATTERN rertst-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_test/win-def:object" priority="4000"
                 mode="M814">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:regkeyeffectiverights_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a regkeyeffectiverights_test must reference a regkeyeffectiverights_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M814"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_test/win-def:state" priority="3999"
                 mode="M814">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:regkeyeffectiverights_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a regkeyeffectiverights_test must reference a regkeyeffectiverights_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M814"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M814"/>
   <xsl:template match="@*|node()" priority="-2" mode="M814">
      <xsl:apply-templates select="@*|node()" mode="M814"/>
   </xsl:template>

   <!--PATTERN rerobjhive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_object/win-def:hive" priority="4000"
                 mode="M815">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M815"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M815"/>
   <xsl:template match="@*|node()" priority="-2" mode="M815">
      <xsl:apply-templates select="@*|node()" mode="M815"/>
   </xsl:template>

   <!--PATTERN rerobjkey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_object/win-def:key" priority="4000"
                 mode="M816">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M816"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M816"/>
   <xsl:template match="@*|node()" priority="-2" mode="M816">
      <xsl:apply-templates select="@*|node()" mode="M816"/>
   </xsl:template>

   <!--PATTERN rerobjtrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_object/win-def:trustee_name"
                 priority="4000"
                 mode="M817">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a regkeyeffectiverights_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M817"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M817"/>
   <xsl:template match="@*|node()" priority="-2" mode="M817">
      <xsl:apply-templates select="@*|node()" mode="M817"/>
   </xsl:template>

   <!--PATTERN rerstehive-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:hive" priority="4000"
                 mode="M818">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the hive entity of a regkeyeffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M818"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M818"/>
   <xsl:template match="@*|node()" priority="-2" mode="M818">
      <xsl:apply-templates select="@*|node()" mode="M818"/>
   </xsl:template>

   <!--PATTERN rerstekey-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key" priority="4000"
                 mode="M819">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key entity of a regkeyeffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M819"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M819"/>
   <xsl:template match="@*|node()" priority="-2" mode="M819">
      <xsl:apply-templates select="@*|node()" mode="M819"/>
   </xsl:template>

   <!--PATTERN rerstetrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:trustee_name"
                 priority="4000"
                 mode="M820">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a regkeyeffectiverights_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M820"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M820"/>
   <xsl:template match="@*|node()" priority="-2" mode="M820">
      <xsl:apply-templates select="@*|node()" mode="M820"/>
   </xsl:template>

   <!--PATTERN rerstestandard_delete-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_delete"
                 priority="4000"
                 mode="M821">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_delete entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M821"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M821"/>
   <xsl:template match="@*|node()" priority="-2" mode="M821">
      <xsl:apply-templates select="@*|node()" mode="M821"/>
   </xsl:template>

   <!--PATTERN rerstestandard_read_control-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_read_control"
                 priority="4000"
                 mode="M822">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_read_control entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M822"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M822"/>
   <xsl:template match="@*|node()" priority="-2" mode="M822">
      <xsl:apply-templates select="@*|node()" mode="M822"/>
   </xsl:template>

   <!--PATTERN rerstestandard_write_dac-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_write_dac"
                 priority="4000"
                 mode="M823">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_dac entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M823"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M823"/>
   <xsl:template match="@*|node()" priority="-2" mode="M823">
      <xsl:apply-templates select="@*|node()" mode="M823"/>
   </xsl:template>

   <!--PATTERN rerstestandard_write_owner-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_write_owner"
                 priority="4000"
                 mode="M824">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_write_owner entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M824"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M824"/>
   <xsl:template match="@*|node()" priority="-2" mode="M824">
      <xsl:apply-templates select="@*|node()" mode="M824"/>
   </xsl:template>

   <!--PATTERN rerstestandard_synchronize-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:standard_synchronize"
                 priority="4000"
                 mode="M825">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the standard_synchronize entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M825"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M825"/>
   <xsl:template match="@*|node()" priority="-2" mode="M825">
      <xsl:apply-templates select="@*|node()" mode="M825"/>
   </xsl:template>

   <!--PATTERN rersteaccess_system_security-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:access_system_security"
                 priority="4000"
                 mode="M826">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_system_security entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M826"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M826"/>
   <xsl:template match="@*|node()" priority="-2" mode="M826">
      <xsl:apply-templates select="@*|node()" mode="M826"/>
   </xsl:template>

   <!--PATTERN rerstegeneric_read-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_read"
                 priority="4000"
                 mode="M827">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_read entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M827"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M827"/>
   <xsl:template match="@*|node()" priority="-2" mode="M827">
      <xsl:apply-templates select="@*|node()" mode="M827"/>
   </xsl:template>

   <!--PATTERN rerstegeneric_write-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_write"
                 priority="4000"
                 mode="M828">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_write entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M828"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M828"/>
   <xsl:template match="@*|node()" priority="-2" mode="M828">
      <xsl:apply-templates select="@*|node()" mode="M828"/>
   </xsl:template>

   <!--PATTERN rerstegeneric_execute-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_execute"
                 priority="4000"
                 mode="M829">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_execute entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M829"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M829"/>
   <xsl:template match="@*|node()" priority="-2" mode="M829">
      <xsl:apply-templates select="@*|node()" mode="M829"/>
   </xsl:template>

   <!--PATTERN rerstegeneric_all-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:generic_all" priority="4000"
                 mode="M830">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the generic_all entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M830"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M830"/>
   <xsl:template match="@*|node()" priority="-2" mode="M830">
      <xsl:apply-templates select="@*|node()" mode="M830"/>
   </xsl:template>

   <!--PATTERN rerstekey_query_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_query_value"
                 priority="4000"
                 mode="M831">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_query_value entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M831"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M831"/>
   <xsl:template match="@*|node()" priority="-2" mode="M831">
      <xsl:apply-templates select="@*|node()" mode="M831"/>
   </xsl:template>

   <!--PATTERN rerstekey_set_value-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_set_value"
                 priority="4000"
                 mode="M832">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_set_value entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M832"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M832"/>
   <xsl:template match="@*|node()" priority="-2" mode="M832">
      <xsl:apply-templates select="@*|node()" mode="M832"/>
   </xsl:template>

   <!--PATTERN rerstekey_create_sub_key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_create_sub_key"
                 priority="4000"
                 mode="M833">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_sub_key entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M833"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M833"/>
   <xsl:template match="@*|node()" priority="-2" mode="M833">
      <xsl:apply-templates select="@*|node()" mode="M833"/>
   </xsl:template>

   <!--PATTERN rerstekey_enumerate_sub_keys-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_enumerate_sub_keys"
                 priority="4000"
                 mode="M834">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_enumerate_sub_keys entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M834"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M834"/>
   <xsl:template match="@*|node()" priority="-2" mode="M834">
      <xsl:apply-templates select="@*|node()" mode="M834"/>
   </xsl:template>

   <!--PATTERN rerstekey_notify-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_notify" priority="4000"
                 mode="M835">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_notify entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M835"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M835"/>
   <xsl:template match="@*|node()" priority="-2" mode="M835">
      <xsl:apply-templates select="@*|node()" mode="M835"/>
   </xsl:template>

   <!--PATTERN rerstekey_create_link-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_create_link"
                 priority="4000"
                 mode="M836">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_create_link entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M836"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M836"/>
   <xsl:template match="@*|node()" priority="-2" mode="M836">
      <xsl:apply-templates select="@*|node()" mode="M836"/>
   </xsl:template>

   <!--PATTERN rerstekey_wow64_64key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_wow64_64key"
                 priority="4000"
                 mode="M837">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_64key entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M837"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M837"/>
   <xsl:template match="@*|node()" priority="-2" mode="M837">
      <xsl:apply-templates select="@*|node()" mode="M837"/>
   </xsl:template>

   <!--PATTERN rerstekey_wow64_32key-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_wow64_32key"
                 priority="4000"
                 mode="M838">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_32key entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M838"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M838"/>
   <xsl:template match="@*|node()" priority="-2" mode="M838">
      <xsl:apply-templates select="@*|node()" mode="M838"/>
   </xsl:template>

   <!--PATTERN rerstekey_wow64_res-->


	<!--RULE -->
<xsl:template match="win-def:regkeyeffectiverights_state/win-def:key_wow64_res"
                 priority="4000"
                 mode="M839">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the key_wow64_res entity of a regkeyeffectiverights_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M839"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M839"/>
   <xsl:template match="@*|node()" priority="-2" mode="M839">
      <xsl:apply-templates select="@*|node()" mode="M839"/>
   </xsl:template>

   <!--PATTERN srtst-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_test/win-def:object" priority="4000" mode="M840">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:sharedresource_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a sharedresource_test must reference a sharedresource_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M840"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:sharedresource_test/win-def:state" priority="3999" mode="M840">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:sharedresource_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a sharedresource_test must reference a sharedresource_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M840"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M840"/>
   <xsl:template match="@*|node()" priority="-2" mode="M840">
      <xsl:apply-templates select="@*|node()" mode="M840"/>
   </xsl:template>

   <!--PATTERN srobjnetname-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_object/win-def:netname" priority="4000"
                 mode="M841">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the netname entity of a sharedresource_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M841"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M841"/>
   <xsl:template match="@*|node()" priority="-2" mode="M841">
      <xsl:apply-templates select="@*|node()" mode="M841"/>
   </xsl:template>

   <!--PATTERN srstenetname-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:netname" priority="4000"
                 mode="M842">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the netname entity of a sharedresource_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M842"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M842"/>
   <xsl:template match="@*|node()" priority="-2" mode="M842">
      <xsl:apply-templates select="@*|node()" mode="M842"/>
   </xsl:template>

   <!--PATTERN srsteshared_type-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:shared_type" priority="4000"
                 mode="M843">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the shared_type entity of a sharedresource_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M843"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M843"/>
   <xsl:template match="@*|node()" priority="-2" mode="M843">
      <xsl:apply-templates select="@*|node()" mode="M843"/>
   </xsl:template>

   <!--PATTERN srstemax_uses-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:max_uses" priority="4000"
                 mode="M844">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the max_uses entity of a sharedresource_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M844"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M844"/>
   <xsl:template match="@*|node()" priority="-2" mode="M844">
      <xsl:apply-templates select="@*|node()" mode="M844"/>
   </xsl:template>

   <!--PATTERN srstecurrent_uses-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:current_uses" priority="4000"
                 mode="M845">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the current_uses entity of a sharedresource_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M845"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M845"/>
   <xsl:template match="@*|node()" priority="-2" mode="M845">
      <xsl:apply-templates select="@*|node()" mode="M845"/>
   </xsl:template>

   <!--PATTERN srstelocal_path-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:local_path" priority="4000"
                 mode="M846">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the local_path entity of a sharedresource_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M846"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M846"/>
   <xsl:template match="@*|node()" priority="-2" mode="M846">
      <xsl:apply-templates select="@*|node()" mode="M846"/>
   </xsl:template>

   <!--PATTERN srsteaccess_read_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_read_permission"
                 priority="4000"
                 mode="M847">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_read_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M847"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M847"/>
   <xsl:template match="@*|node()" priority="-2" mode="M847">
      <xsl:apply-templates select="@*|node()" mode="M847"/>
   </xsl:template>

   <!--PATTERN srsteaccess_write_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_write_permission"
                 priority="4000"
                 mode="M848">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_write_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M848"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M848"/>
   <xsl:template match="@*|node()" priority="-2" mode="M848">
      <xsl:apply-templates select="@*|node()" mode="M848"/>
   </xsl:template>

   <!--PATTERN srsteaccess_create_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_create_permission"
                 priority="4000"
                 mode="M849">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_create_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M849"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M849"/>
   <xsl:template match="@*|node()" priority="-2" mode="M849">
      <xsl:apply-templates select="@*|node()" mode="M849"/>
   </xsl:template>

   <!--PATTERN srsteaccess_exec_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_exec_permission"
                 priority="4000"
                 mode="M850">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_exec_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M850"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M850"/>
   <xsl:template match="@*|node()" priority="-2" mode="M850">
      <xsl:apply-templates select="@*|node()" mode="M850"/>
   </xsl:template>

   <!--PATTERN srsteaccess_delete_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_delete_permission"
                 priority="4000"
                 mode="M851">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_delete_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M851"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M851"/>
   <xsl:template match="@*|node()" priority="-2" mode="M851">
      <xsl:apply-templates select="@*|node()" mode="M851"/>
   </xsl:template>

   <!--PATTERN srsteaccess_atrib_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_atrib_permission"
                 priority="4000"
                 mode="M852">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_atrib_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M852"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M852"/>
   <xsl:template match="@*|node()" priority="-2" mode="M852">
      <xsl:apply-templates select="@*|node()" mode="M852"/>
   </xsl:template>

   <!--PATTERN srsteaccess_perm_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_perm_permission"
                 priority="4000"
                 mode="M853">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_perm_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M853"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M853"/>
   <xsl:template match="@*|node()" priority="-2" mode="M853">
      <xsl:apply-templates select="@*|node()" mode="M853"/>
   </xsl:template>

   <!--PATTERN srsteaccess_all_permission-->


	<!--RULE -->
<xsl:template match="win-def:sharedresource_state/win-def:access_all_permission"
                 priority="4000"
                 mode="M854">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='bool'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the access_all_permission entity of a sharedresource_state should be 'bool'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M854"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M854"/>
   <xsl:template match="@*|node()" priority="-2" mode="M854">
      <xsl:apply-templates select="@*|node()" mode="M854"/>
   </xsl:template>

   <!--PATTERN sidtst-->


	<!--RULE -->
<xsl:template match="win-def:sid_test/win-def:object" priority="4000" mode="M855">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:sid_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a sid_test must reference a sid_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M855"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:sid_test/win-def:state" priority="3999" mode="M855">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:sid_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a sid_test must reference a sid_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M855"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M855"/>
   <xsl:template match="@*|node()" priority="-2" mode="M855">
      <xsl:apply-templates select="@*|node()" mode="M855"/>
   </xsl:template>

   <!--PATTERN sidobjtrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:sid_object/win-def:trustee_name" priority="4000" mode="M856">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a sid_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M856"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M856"/>
   <xsl:template match="@*|node()" priority="-2" mode="M856">
      <xsl:apply-templates select="@*|node()" mode="M856"/>
   </xsl:template>

   <!--PATTERN sidstetrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:sid_state/win-def:trustee_name" priority="4000" mode="M857">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M857"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M857"/>
   <xsl:template match="@*|node()" priority="-2" mode="M857">
      <xsl:apply-templates select="@*|node()" mode="M857"/>
   </xsl:template>

   <!--PATTERN sidstetrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:sid_state/win-def:trustee_sid" priority="4000" mode="M858">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M858"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M858"/>
   <xsl:template match="@*|node()" priority="-2" mode="M858">
      <xsl:apply-templates select="@*|node()" mode="M858"/>
   </xsl:template>

   <!--PATTERN sidstetrustee_domain-->


	<!--RULE -->
<xsl:template match="win-def:sid_state/win-def:trustee_domain" priority="4000" mode="M859">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_domain entity of a sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M859"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M859"/>
   <xsl:template match="@*|node()" priority="-2" mode="M859">
      <xsl:apply-templates select="@*|node()" mode="M859"/>
   </xsl:template>

   <!--PATTERN sidsidtst-->


	<!--RULE -->
<xsl:template match="win-def:sid_sid_test/win-def:object" priority="4000" mode="M860">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:sid_sid_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a sid_sid_test must reference a sid_sid_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M860"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:sid_sid_test/win-def:state" priority="3999" mode="M860">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:sid_sid_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a sid_sid_test must reference a sid_sid_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M860"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M860"/>
   <xsl:template match="@*|node()" priority="-2" mode="M860">
      <xsl:apply-templates select="@*|node()" mode="M860"/>
   </xsl:template>

   <!--PATTERN sidsidobjtrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:sid_sid_object/win-def:trustee_sid" priority="4000" mode="M861">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a sid_sid_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M861"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M861"/>
   <xsl:template match="@*|node()" priority="-2" mode="M861">
      <xsl:apply-templates select="@*|node()" mode="M861"/>
   </xsl:template>

   <!--PATTERN sidsidstetrustee_sid-->


	<!--RULE -->
<xsl:template match="win-def:sid_sid_state/win-def:trustee_sid" priority="4000" mode="M862">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_sid entity of a sid_sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M862"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M862"/>
   <xsl:template match="@*|node()" priority="-2" mode="M862">
      <xsl:apply-templates select="@*|node()" mode="M862"/>
   </xsl:template>

   <!--PATTERN sidsidstetrustee_name-->


	<!--RULE -->
<xsl:template match="win-def:sid_sid_state/win-def:trustee_name" priority="4000" mode="M863">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_name entity of a sid_sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M863"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M863"/>
   <xsl:template match="@*|node()" priority="-2" mode="M863">
      <xsl:apply-templates select="@*|node()" mode="M863"/>
   </xsl:template>

   <!--PATTERN sidsidstetrustee_domain-->


	<!--RULE -->
<xsl:template match="win-def:sid_sid_state/win-def:trustee_domain" priority="4000"
                 mode="M864">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the trustee_domain entity of a sid_sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M864"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M864"/>
   <xsl:template match="@*|node()" priority="-2" mode="M864">
      <xsl:apply-templates select="@*|node()" mode="M864"/>
   </xsl:template>

   <!--PATTERN uactst-->


	<!--RULE -->
<xsl:template match="win-def:uac_test/win-def:object" priority="4000" mode="M865">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:uac_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a uac_test must reference a uac_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M865"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:uac_test/win-def:state" priority="3999" mode="M865">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:uac_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a uac_test must reference a uac_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M865"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M865"/>
   <xsl:template match="@*|node()" priority="-2" mode="M865">
      <xsl:apply-templates select="@*|node()" mode="M865"/>
   </xsl:template>

   <!--PATTERN uacsteadminapprovalmode-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:admin_approval_mode" priority="4000"
                 mode="M866">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the admin_approval_mode entity of a uac_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M866"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M866"/>
   <xsl:template match="@*|node()" priority="-2" mode="M866">
      <xsl:apply-templates select="@*|node()" mode="M866"/>
   </xsl:template>

   <!--PATTERN uacstebehaviorelevationadmin-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:elevation_prompt_admin" priority="4000"
                 mode="M867">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the elevation_prompt_admin entity of a uac_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M867"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M867"/>
   <xsl:template match="@*|node()" priority="-2" mode="M867">
      <xsl:apply-templates select="@*|node()" mode="M867"/>
   </xsl:template>

   <!--PATTERN uacsteelevationpromptstandard-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:elevation_prompt_standard" priority="4000"
                 mode="M868">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the elevation_prompt_standard entity of a uac_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M868"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M868"/>
   <xsl:template match="@*|node()" priority="-2" mode="M868">
      <xsl:apply-templates select="@*|node()" mode="M868"/>
   </xsl:template>

   <!--PATTERN uacstedetectinstallations-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:detect_installations" priority="4000"
                 mode="M869">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the detect_installations entity of a uac_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M869"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M869"/>
   <xsl:template match="@*|node()" priority="-2" mode="M869">
      <xsl:apply-templates select="@*|node()" mode="M869"/>
   </xsl:template>

   <!--PATTERN uacsteelevatesignedexecutables-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:elevate_signed_executables" priority="4000"
                 mode="M870">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the elevate_signed_executables entity of a uac_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M870"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M870"/>
   <xsl:template match="@*|node()" priority="-2" mode="M870">
      <xsl:apply-templates select="@*|node()" mode="M870"/>
   </xsl:template>

   <!--PATTERN uacsteelevateuiaccess-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:elevate_uiaccess" priority="4000" mode="M871">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the elevate_uiaccess entity of a uac_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M871"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M871"/>
   <xsl:template match="@*|node()" priority="-2" mode="M871">
      <xsl:apply-templates select="@*|node()" mode="M871"/>
   </xsl:template>

   <!--PATTERN uacsterunadminsaam-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:run_admins_aam" priority="4000" mode="M872">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the run_admins_aam entity of a uac_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M872"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M872"/>
   <xsl:template match="@*|node()" priority="-2" mode="M872">
      <xsl:apply-templates select="@*|node()" mode="M872"/>
   </xsl:template>

   <!--PATTERN uacstesecuredesktop-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:secure_desktop" priority="4000" mode="M873">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the secure_desktop entity of a uac_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M873"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M873"/>
   <xsl:template match="@*|node()" priority="-2" mode="M873">
      <xsl:apply-templates select="@*|node()" mode="M873"/>
   </xsl:template>

   <!--PATTERN uacstevirtualizewritefailures-->


	<!--RULE -->
<xsl:template match="win-def:uac_state/win-def:virtualize_write_failures" priority="4000"
                 mode="M874">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the virtualize_write_failures entity of a uac_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M874"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M874"/>
   <xsl:template match="@*|node()" priority="-2" mode="M874">
      <xsl:apply-templates select="@*|node()" mode="M874"/>
   </xsl:template>

   <!--PATTERN usertst-->


	<!--RULE -->
<xsl:template match="win-def:user_test/win-def:object" priority="4000" mode="M875">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:user_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a user_test must reference a user_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M875"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:user_test/win-def:state" priority="3999" mode="M875">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:user_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a user_test must reference a user_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M875"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M875"/>
   <xsl:template match="@*|node()" priority="-2" mode="M875">
      <xsl:apply-templates select="@*|node()" mode="M875"/>
   </xsl:template>

   <!--PATTERN userobjuser-->


	<!--RULE -->
<xsl:template match="win-def:user_object/win-def:user" priority="4000" mode="M876">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user entity of a user_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M876"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M876"/>
   <xsl:template match="@*|node()" priority="-2" mode="M876">
      <xsl:apply-templates select="@*|node()" mode="M876"/>
   </xsl:template>

   <!--PATTERN usersteuser-->


	<!--RULE -->
<xsl:template match="win-def:user_state/win-def:user" priority="4000" mode="M877">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user entity of a user_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M877"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M877"/>
   <xsl:template match="@*|node()" priority="-2" mode="M877">
      <xsl:apply-templates select="@*|node()" mode="M877"/>
   </xsl:template>

   <!--PATTERN usersteenabled-->


	<!--RULE -->
<xsl:template match="win-def:user_state/win-def:enabled" priority="4000" mode="M878">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the enabled entity of a user_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M878"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M878"/>
   <xsl:template match="@*|node()" priority="-2" mode="M878">
      <xsl:apply-templates select="@*|node()" mode="M878"/>
   </xsl:template>

   <!--PATTERN userstegroup-->


	<!--RULE -->
<xsl:template match="win-def:user_state/win-def:group" priority="4000" mode="M879">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group entity of a user_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M879"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M879"/>
   <xsl:template match="@*|node()" priority="-2" mode="M879">
      <xsl:apply-templates select="@*|node()" mode="M879"/>
   </xsl:template>

   <!--PATTERN usersidtst-->


	<!--RULE -->
<xsl:template match="win-def:user_sid_test/win-def:object" priority="4000" mode="M880">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:user_sid_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a user_sid_test must reference a user_sid_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M880"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:user_sid_test/win-def:state" priority="3999" mode="M880">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:user_sid_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a user_sid_test must reference a user_sid_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M880"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M880"/>
   <xsl:template match="@*|node()" priority="-2" mode="M880">
      <xsl:apply-templates select="@*|node()" mode="M880"/>
   </xsl:template>

   <!--PATTERN usersidobjuser-->


	<!--RULE -->
<xsl:template match="win-def:user_sid_object/win-def:user_sid" priority="4000" mode="M881">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_sid entity of a user_sid_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M881"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M881"/>
   <xsl:template match="@*|node()" priority="-2" mode="M881">
      <xsl:apply-templates select="@*|node()" mode="M881"/>
   </xsl:template>

   <!--PATTERN usersidsteuser-->


	<!--RULE -->
<xsl:template match="win-def:user_sid_state/win-def:user_sid" priority="4000" mode="M882">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the user_sid entity of a user_sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M882"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M882"/>
   <xsl:template match="@*|node()" priority="-2" mode="M882">
      <xsl:apply-templates select="@*|node()" mode="M882"/>
   </xsl:template>

   <!--PATTERN usersidsteenabled-->


	<!--RULE -->
<xsl:template match="win-def:user_sid_state/win-def:enabled" priority="4000" mode="M883">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the enabled entity of a user_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M883"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M883"/>
   <xsl:template match="@*|node()" priority="-2" mode="M883">
      <xsl:apply-templates select="@*|node()" mode="M883"/>
   </xsl:template>

   <!--PATTERN usersidstegroup-->


	<!--RULE -->
<xsl:template match="win-def:user_sid_state/win-def:group_sid" priority="4000" mode="M884">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the group_sid entity of a user_sid_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M884"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M884"/>
   <xsl:template match="@*|node()" priority="-2" mode="M884">
      <xsl:apply-templates select="@*|node()" mode="M884"/>
   </xsl:template>

   <!--PATTERN volumetst-->


	<!--RULE -->
<xsl:template match="win-def:volume_test/win-def:object" priority="4000" mode="M885">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:volume_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a volume_test must reference a volume_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M885"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:volume_test/win-def:state" priority="3999" mode="M885">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:volume_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a volume_test must reference a volume_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M885"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M885"/>
   <xsl:template match="@*|node()" priority="-2" mode="M885">
      <xsl:apply-templates select="@*|node()" mode="M885"/>
   </xsl:template>

   <!--PATTERN volobjrootpath-->


	<!--RULE -->
<xsl:template match="win-def:volume_object/win-def:rootpath" priority="4000" mode="M886">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the rootpath entity of a volume_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M886"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M886"/>
   <xsl:template match="@*|node()" priority="-2" mode="M886">
      <xsl:apply-templates select="@*|node()" mode="M886"/>
   </xsl:template>

   <!--PATTERN volumesterootpath-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:rootpath" priority="4000" mode="M887">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the rootpath entity of a volume_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M887"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M887"/>
   <xsl:template match="@*|node()" priority="-2" mode="M887">
      <xsl:apply-templates select="@*|node()" mode="M887"/>
   </xsl:template>

   <!--PATTERN volumestefile_system-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_system" priority="4000" mode="M888">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_system entity of a volume_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M888"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M888"/>
   <xsl:template match="@*|node()" priority="-2" mode="M888">
      <xsl:apply-templates select="@*|node()" mode="M888"/>
   </xsl:template>

   <!--PATTERN volumestename-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:name" priority="4000" mode="M889">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the name entity of a volume_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M889"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M889"/>
   <xsl:template match="@*|node()" priority="-2" mode="M889">
      <xsl:apply-templates select="@*|node()" mode="M889"/>
   </xsl:template>

   <!--PATTERN volstevolume_max_component_length-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:volume_max_component_length"
                 priority="4000"
                 mode="M890">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the volume_max_component_length entity of a volume_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M890"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M890"/>
   <xsl:template match="@*|node()" priority="-2" mode="M890">
      <xsl:apply-templates select="@*|node()" mode="M890"/>
   </xsl:template>

   <!--PATTERN volsteserial_number-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:serial_number" priority="4000" mode="M891">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='int'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the serial_number entity of a volume_state should be 'int'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M891"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M891"/>
   <xsl:template match="@*|node()" priority="-2" mode="M891">
      <xsl:apply-templates select="@*|node()" mode="M891"/>
   </xsl:template>

   <!--PATTERN volstefile_case_sensitive_search-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_case_sensitive_search" priority="4000"
                 mode="M892">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_case_sensitive_search entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M892"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M892"/>
   <xsl:template match="@*|node()" priority="-2" mode="M892">
      <xsl:apply-templates select="@*|node()" mode="M892"/>
   </xsl:template>

   <!--PATTERN volstefile_case_preserved_names-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_case_preserved_names" priority="4000"
                 mode="M893">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_case_preserved_names entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M893"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M893"/>
   <xsl:template match="@*|node()" priority="-2" mode="M893">
      <xsl:apply-templates select="@*|node()" mode="M893"/>
   </xsl:template>

   <!--PATTERN volstefile_unicode_on_disk-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_unicode_on_disk" priority="4000"
                 mode="M894">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_unicode_on_disk entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M894"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M894"/>
   <xsl:template match="@*|node()" priority="-2" mode="M894">
      <xsl:apply-templates select="@*|node()" mode="M894"/>
   </xsl:template>

   <!--PATTERN volstefile_persistent_acls-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_persistent_acls" priority="4000"
                 mode="M895">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_persistent_acls entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M895"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M895"/>
   <xsl:template match="@*|node()" priority="-2" mode="M895">
      <xsl:apply-templates select="@*|node()" mode="M895"/>
   </xsl:template>

   <!--PATTERN volstefile_file_compression-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_file_compression" priority="4000"
                 mode="M896">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_file_compression entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M896"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M896"/>
   <xsl:template match="@*|node()" priority="-2" mode="M896">
      <xsl:apply-templates select="@*|node()" mode="M896"/>
   </xsl:template>

   <!--PATTERN volstefile_volume_quotas-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_volume_quotas" priority="4000"
                 mode="M897">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_volume_quotas entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M897"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M897"/>
   <xsl:template match="@*|node()" priority="-2" mode="M897">
      <xsl:apply-templates select="@*|node()" mode="M897"/>
   </xsl:template>

   <!--PATTERN volstefile_supports_sparse_files-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_supports_sparse_files" priority="4000"
                 mode="M898">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_supports_sparse_files entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M898"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M898"/>
   <xsl:template match="@*|node()" priority="-2" mode="M898">
      <xsl:apply-templates select="@*|node()" mode="M898"/>
   </xsl:template>

   <!--PATTERN volstefile_supports_reparse_points-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_supports_reparse_points"
                 priority="4000"
                 mode="M899">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_supports_reparse_points entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M899"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M899"/>
   <xsl:template match="@*|node()" priority="-2" mode="M899">
      <xsl:apply-templates select="@*|node()" mode="M899"/>
   </xsl:template>

   <!--PATTERN volstefile_supports_remote_storage-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_supports_remote_storage"
                 priority="4000"
                 mode="M900">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_supports_remote_storage entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M900"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M900"/>
   <xsl:template match="@*|node()" priority="-2" mode="M900">
      <xsl:apply-templates select="@*|node()" mode="M900"/>
   </xsl:template>

   <!--PATTERN volstefile_volume_is_compressed-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_volume_is_compressed" priority="4000"
                 mode="M901">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_volume_is_compressed entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M901"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M901"/>
   <xsl:template match="@*|node()" priority="-2" mode="M901">
      <xsl:apply-templates select="@*|node()" mode="M901"/>
   </xsl:template>

   <!--PATTERN volstefile_supports_object_ids-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_supports_object_ids" priority="4000"
                 mode="M902">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_supports_object_ids entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M902"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M902"/>
   <xsl:template match="@*|node()" priority="-2" mode="M902">
      <xsl:apply-templates select="@*|node()" mode="M902"/>
   </xsl:template>

   <!--PATTERN volstefile_supports_encryption-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_supports_encryption" priority="4000"
                 mode="M903">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_supports_encryption entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M903"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M903"/>
   <xsl:template match="@*|node()" priority="-2" mode="M903">
      <xsl:apply-templates select="@*|node()" mode="M903"/>
   </xsl:template>

   <!--PATTERN volstefile_named_streams-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_named_streams" priority="4000"
                 mode="M904">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_named_streams entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M904"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M904"/>
   <xsl:template match="@*|node()" priority="-2" mode="M904">
      <xsl:apply-templates select="@*|node()" mode="M904"/>
   </xsl:template>

   <!--PATTERN volstefile_read_only_volume-->


	<!--RULE -->
<xsl:template match="win-def:volume_state/win-def:file_read_only_volume" priority="4000"
                 mode="M905">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@datatype='boolean'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the file_read_only_volume entity of a volume_state should be 'boolean'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M905"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M905"/>
   <xsl:template match="@*|node()" priority="-2" mode="M905">
      <xsl:apply-templates select="@*|node()" mode="M905"/>
   </xsl:template>

   <!--PATTERN wmitst-->


	<!--RULE -->
<xsl:template match="win-def:wmi_test/win-def:object" priority="4000" mode="M906">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@object_ref=/oval-def:oval_definitions/oval-def:objects/win-def:wmi_object/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the object child element of a wmi_test must reference a wmi_object<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M906"/>
   </xsl:template>

	  <!--RULE -->
<xsl:template match="win-def:wmi_test/win-def:state" priority="3999" mode="M906">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="@state_ref=/oval-def:oval_definitions/oval-def:states/win-def:wmi_state/@id"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - the state child element of a wmi_test must reference a wmi_state<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M906"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M906"/>
   <xsl:template match="@*|node()" priority="-2" mode="M906">
      <xsl:apply-templates select="@*|node()" mode="M906"/>
   </xsl:template>

   <!--PATTERN wmiobjnamespace-->


	<!--RULE -->
<xsl:template match="win-def:wmi_object/win-def:namespace" priority="4000" mode="M907">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the namespace entity of a wmi_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M907"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M907"/>
   <xsl:template match="@*|node()" priority="-2" mode="M907">
      <xsl:apply-templates select="@*|node()" mode="M907"/>
   </xsl:template>

   <!--PATTERN wmiobjwql-->


	<!--RULE -->
<xsl:template match="win-def:wmi_object/win-def:wql" priority="4000" mode="M908">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the wql entity of a wmi_object should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M908"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M908"/>
   <xsl:template match="@*|node()" priority="-2" mode="M908">
      <xsl:apply-templates select="@*|node()" mode="M908"/>
   </xsl:template>

   <!--PATTERN wmistenamespace-->


	<!--RULE -->
<xsl:template match="win-def:wmi_state/win-def:namespace" priority="4000" mode="M909">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the namespace entity of a wmi_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M909"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M909"/>
   <xsl:template match="@*|node()" priority="-2" mode="M909">
      <xsl:apply-templates select="@*|node()" mode="M909"/>
   </xsl:template>

   <!--PATTERN wmistewql-->


	<!--RULE -->
<xsl:template match="win-def:wmi_state/win-def:wql" priority="4000" mode="M910">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="not(@datatype) or @datatype='string'"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - datatype attribute for the wql entity of a wmi_state should be 'string'<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M910"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M910"/>
   <xsl:template match="@*|node()" priority="-2" mode="M910">
      <xsl:apply-templates select="@*|node()" mode="M910"/>
   </xsl:template>

   <!--PATTERN wmisteresult-->


	<!--RULE -->
<xsl:template match="win-def:wmi_state/win-def:result" priority="4000" mode="M911">

		<!--ASSERT -->
<xsl:choose>
         <xsl:when test="(@datatype='int' and (floor(.) = number(.))) or not(@datatype='int') or not(node())"/>
         <xsl:otherwise>
                        <xsl:text/>
            <xsl:value-of select="../@id"/>
            <xsl:text/> - The datatype has been set to 'int' but the value is not an integer.<xsl:value-of select="string('&#xA;')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="@*|*|comment()|processing-instruction()" mode="M911"/>
   </xsl:template>
   <xsl:template match="text()" priority="-1" mode="M911"/>
   <xsl:template match="@*|node()" priority="-2" mode="M911">
      <xsl:apply-templates select="@*|node()" mode="M911"/>
   </xsl:template>
</xsl:stylesheet>