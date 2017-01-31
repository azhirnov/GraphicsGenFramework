// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "IsoLocale.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	_Instance
=================================================
*/
	inline IsoLocale * IsoLocale::_Instance ()
	{
		return SingletonMultiThread::Instance< IsoLocale >();
	}

/*
=================================================
	Iso2ToIso3
=================================================
*/
	Iso3Locale IsoLocale::Iso2ToIso3 (const Iso2Locale &iso2)
	{
		iso_pair_array_t const &	arr = _Instance()->_isoPairs;

		FOR( i, arr )
		{
			if ( arr[i].iso2 == iso2 )
				return arr[i].iso3;
		}
		return Iso3Locale();
	}
	
/*
=================================================
	Iso3ToIso2
=================================================
*/
	Iso2Locale IsoLocale::Iso3ToIso2 (const Iso3Locale &iso3)
	{
		iso_pair_array_t const &	arr = _Instance()->_isoPairs;

		FOR( i, arr )
		{
			if ( arr[i].iso3 == iso3 )
				return arr[i].iso2;
		}
		return Iso2Locale();
	}
	
/*
=================================================
	GetCode
=================================================
*/
	uint IsoLocale::GetCode (const Iso2Locale &iso2)
	{
		iso_pair_array_t const &	arr = _Instance()->_isoPairs;

		FOR( i, arr )
		{
			if ( arr[i].iso2 == iso2 )
				return arr[i].code;
		}
		return 0;
	}
	
/*
=================================================
	GetCode
=================================================
*/
	uint IsoLocale::GetCode (const Iso3Locale &iso3)
	{
		iso_pair_array_t const &	arr = _Instance()->_isoPairs;

		FOR( i, arr )
		{
			if ( arr[i].iso3 == iso3 )
				return arr[i].code;
		}
		return 0;
	}

/*
=================================================
	Load
=================================================
*/
	IsoLocale::IsoLocale (UninitializedType)
	{
		iso_pair_array_t &	arr = _isoPairs;
		uint				i	= 0;

		arr[i++].Set( "AF", "AFG",   4 );		// AFGHANISTAN
		arr[i++].Set( "AL", "ALB",   8 );		// ALBANIA
		arr[i++].Set( "DZ", "DZA",  12 );		// ALGERIA
		arr[i++].Set( "AS", "ASM",  16 );		// AMERICAN SAMOA
		arr[i++].Set( "AD", "AND",  20 );		// ANDORRA
		arr[i++].Set( "AO", "AGO",  24 );		// ANGOLA
		arr[i++].Set( "AI", "AIA", 660 );		// ANGUILLA
		arr[i++].Set( "AQ", "ATA",  10 );		// ANTARCTICA
		arr[i++].Set( "AG", "ATG",  28 );		// ANTIGUA AND BARBUDA
		arr[i++].Set( "AR", "ARG",  32 );		// ARGENTINA
		arr[i++].Set( "AM", "ARM",  51 );		// ARMENIA
		arr[i++].Set( "AW", "ABW", 533 );		// ARUBA
		arr[i++].Set( "AU", "AUS",  36 );		// AUSTRALIA
		arr[i++].Set( "AT", "AUT",  40 );		// AUSTRIA
		arr[i++].Set( "AZ", "AZE",  31 );		// AZERBAIJAN
		arr[i++].Set( "BS", "BHS",  44 );		// BAHAMAS
		arr[i++].Set( "BH", "BHR",  48 );		// BAHRAIN
		arr[i++].Set( "BD", "BGD",  50 );		// BANGLADESH
		arr[i++].Set( "BB", "BRB",  52 );		// BARBADOS
		arr[i++].Set( "BY", "BLR", 112 );		// BELARUS
		arr[i++].Set( "BE", "BEL",  56 );		// BELGIUM
		arr[i++].Set( "BZ", "BLZ",  84 );		// BELIZE
		arr[i++].Set( "BJ", "BEN", 204 );		// BENIN
		arr[i++].Set( "BM", "BMU",  60 );		// BERMUDA
		arr[i++].Set( "BT", "BTN",  64 );		// BHUTAN
		arr[i++].Set( "BO", "BOL",  68 );		// BOLIVIA
		arr[i++].Set( "BA", "BIH",  70 );		// BOSNIA AND HERZEGOWINA
		arr[i++].Set( "BW", "BWA",  72 );		// BOTSWANA
		arr[i++].Set( "BV", "BVT",  74 );		// BOUVET ISLAND
		arr[i++].Set( "BR", "BRA",  76 );		// BRAZIL
		arr[i++].Set( "IO", "IOT",  86 );		// BRITISH INDIAN OCEAN TERRITORY
		arr[i++].Set( "BN", "BRN",  96 );		// BRUNEI DARUSSALAM
		arr[i++].Set( "BG", "BGR", 100 );		// BULGARIA
		arr[i++].Set( "BF", "BFA", 854 );		// BURKINA FASO
		arr[i++].Set( "BI", "BDI", 108 );		// BURUNDI
		arr[i++].Set( "KH", "KHM", 116 );		// CAMBODIA
		arr[i++].Set( "CM", "CMR", 120 );		// CAMEROON
		arr[i++].Set( "CA", "CAN", 124 );		// CANADA
		arr[i++].Set( "CV", "CPV", 132 );		// CAPE VERDE
		arr[i++].Set( "KY", "CYM", 136 );		// CAYMAN ISLANDS
		arr[i++].Set( "CF", "CAF", 140 );		// CENTRAL AFRICAN REPUBLIC
		arr[i++].Set( "TD", "TCD", 148 );		// CHAD
		arr[i++].Set( "CL", "CHL", 152 );		// CHILE
		arr[i++].Set( "CN", "CHN", 156 );		// CHINA
		arr[i++].Set( "CX", "CXR", 162 );		// CHRISTMAS ISLAND
		arr[i++].Set( "CC", "CCK", 166 );		// COCOS (KEELING) ISLANDS
		arr[i++].Set( "CO", "COL", 170 );		// COLOMBIA
		arr[i++].Set( "KM", "COM", 174 );		// COMOROS
		arr[i++].Set( "CG", "COG", 178 );		// CONGO
		arr[i++].Set( "CK", "COK", 184 );		// COOK ISLANDS
		arr[i++].Set( "CR", "CRI", 188 );		// COSTA RICA
		arr[i++].Set( "CI", "CIV", 384 );		// COTE D'IVOIRE
		arr[i++].Set( "HR", "HRV", 191 );		// CROATIA (local name: Hrvatska)
		arr[i++].Set( "CU", "CUB", 192 );		// CUBA
		arr[i++].Set( "CY", "CYP", 196 );		// CYPRUS
		arr[i++].Set( "CZ", "CZE", 203 );		// CZECH REPUBLIC
		arr[i++].Set( "DK", "DNK", 208 );		// DENMARK
		arr[i++].Set( "DJ", "DJI", 262 );		// DJIBOUTI
		arr[i++].Set( "DM", "DMA", 212 );		// DOMINICA
		arr[i++].Set( "DO", "DOM", 214 );		// DOMINICAN REPUBLIC
		arr[i++].Set( "TP", "TMP", 626 );		// EAST TIMOR
		arr[i++].Set( "EC", "ECU", 218 );		// ECUADOR
		arr[i++].Set( "EG", "EGY", 818 );		// EGYPT
		arr[i++].Set( "SV", "SLV", 222 );		// EL SALVADOR
		arr[i++].Set( "GQ", "GNQ", 226 );		// EQUATORIAL GUINEA
		arr[i++].Set( "ER", "ERI", 232 );		// ERITREA
		arr[i++].Set( "EE", "EST", 233 );		// ESTONIA
		arr[i++].Set( "ET", "ETH", 231 );		// ETHIOPIA
		arr[i++].Set( "FK", "FLK", 238 );		// FALKLAND ISLANDS (MALVINAS)
		arr[i++].Set( "FO", "FRO", 234 );		// FAROE ISLANDS
		arr[i++].Set( "FJ", "FJI", 242 );		// FIJI
		arr[i++].Set( "FI", "FIN", 246 );		// FINLAND
		arr[i++].Set( "FR", "FRA", 250 );		// FRANCE
		arr[i++].Set( "FX", "FXX", 249 );		// FRANCE, METROPOLITAN
		arr[i++].Set( "GF", "GUF", 254 );		// FRENCH GUIANA
		arr[i++].Set( "PF", "PYF", 258 );		// FRENCH POLYNESIA
		arr[i++].Set( "TF", "ATF", 260 );		// FRENCH SOUTHERN TERRITORIES
		arr[i++].Set( "GA", "GAB", 266 );		// GABON
		arr[i++].Set( "GM", "GMB", 270 );		// GAMBIA
		arr[i++].Set( "GE", "GEO", 268 );		// GEORGIA
		arr[i++].Set( "DE", "DEU", 276 );		// GERMANY
		arr[i++].Set( "GH", "GHA", 288 );		// GHANA
		arr[i++].Set( "GI", "GIB", 292 );		// GIBRALTAR
		arr[i++].Set( "GR", "GRC", 300 );		// GREECE
		arr[i++].Set( "GL", "GRL", 304 );		// GREENLAND
		arr[i++].Set( "GD", "GRD", 308 );		// GRENADA
		arr[i++].Set( "GP", "GLP", 312 );		// GUADELOUPE
		arr[i++].Set( "GU", "GUM", 316 );		// GUAM
		arr[i++].Set( "GT", "GTM", 320 );		// GUATEMALA
		arr[i++].Set( "GN", "GIN", 324 );		// GUINEA
		arr[i++].Set( "GW", "GNB", 624 );		// GUINEA-BISSAU
		arr[i++].Set( "GY", "GUY", 328 );		// GUYANA
		arr[i++].Set( "HT", "HTI", 332 );		// HAITI
		arr[i++].Set( "HM", "HMD", 334 );		// HEARD AND MC DONALD ISLANDS
		arr[i++].Set( "VA", "VAT", 336 );		// HOLY SEE (VATICAN CITY STATE)
		arr[i++].Set( "HN", "HND", 340 );		// HONDURAS
		arr[i++].Set( "HK", "HKG", 344 );		// HONG KONG
		arr[i++].Set( "HU", "HUN", 348 );		// HUNGARY
		arr[i++].Set( "IS", "ISL", 352 );		// ICELAND
		arr[i++].Set( "IN", "IND", 356 );		// INDIA
		arr[i++].Set( "ID", "IDN", 360 );		// INDONESIA
		arr[i++].Set( "IR", "IRN", 364 );		// IRAN (ISLAMIC REPUBLIC OF)
		arr[i++].Set( "IQ", "IRQ", 368 );		// IRAQ
		arr[i++].Set( "IE", "IRL", 372 );		// IRELAND
		arr[i++].Set( "IL", "ISR", 376 );		// ISRAEL
		arr[i++].Set( "IT", "ITA", 380 );		// ITALY
		arr[i++].Set( "JM", "JAM", 388 );		// JAMAICA
		arr[i++].Set( "JP", "JPN", 392 );		// JAPAN
		arr[i++].Set( "JO", "JOR", 400 );		// JORDAN
		arr[i++].Set( "KZ", "KAZ", 398 );		// KAZAKHSTAN
		arr[i++].Set( "KE", "KEN", 404 );		// KENYA
		arr[i++].Set( "KI", "KIR", 296 );		// KIRIBATI
		arr[i++].Set( "KP", "PRK", 408 );		// KOREA, DEMOCRATIC PEOPLE'S REPUBLIC OF
		arr[i++].Set( "KR", "KOR", 410 );		// KOREA, REPUBLIC OF
		arr[i++].Set( "KW", "KWT", 414 );		// KUWAIT
		arr[i++].Set( "KG", "KGZ", 417 );		// KYRGYZSTAN
		arr[i++].Set( "LA", "LAO", 418 );		// LAO PEOPLE'S DEMOCRATIC REPUBLIC
		arr[i++].Set( "LV", "LVA", 428 );		// LATVIA
		arr[i++].Set( "LB", "LBN", 422 );		// LEBANON
		arr[i++].Set( "LS", "LSO", 426 );		// LESOTHO
		arr[i++].Set( "LR", "LBR", 430 );		// LIBERIA
		arr[i++].Set( "LY", "LBY", 434 );		// LIBYAN ARAB JAMAHIRIYA
		arr[i++].Set( "LI", "LIE", 438 );		// LIECHTENSTEIN
		arr[i++].Set( "LT", "LTU", 440 );		// LITHUANIA
		arr[i++].Set( "LU", "LUX", 442 );		// LUXEMBOURG
		arr[i++].Set( "MO", "MAC", 446 );		// MACAU
		arr[i++].Set( "MK", "MKD", 807 );		// MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF
		arr[i++].Set( "MG", "MDG", 450 );		// MADAGASCAR
		arr[i++].Set( "MW", "MWI", 454 );		// MALAWI
		arr[i++].Set( "MY", "MYS", 458 );		// MALAYSIA
		arr[i++].Set( "MV", "MDV", 462 );		// MALDIVES
		arr[i++].Set( "ML", "MLI", 466 );		// MALI
		arr[i++].Set( "MT", "MLT", 470 );		// MALTA
		arr[i++].Set( "MH", "MHL", 584 );		// MARSHALL ISLANDS
		arr[i++].Set( "MQ", "MTQ", 474 );		// MARTINIQUE
		arr[i++].Set( "MR", "MRT", 478 );		// MAURITANIA
		arr[i++].Set( "MU", "MUS", 480 );		// MAURITIUS
		arr[i++].Set( "YT", "MYT", 175 );		// MAYOTTE
		arr[i++].Set( "MX", "MEX", 484 );		// MEXICO
		arr[i++].Set( "FM", "FSM", 583 );		// MICRONESIA, FEDERATED STATES OF
		arr[i++].Set( "MD", "MDA", 498 );		// MOLDOVA, REPUBLIC OF
		arr[i++].Set( "MC", "MCO", 492 );		// MONACO
		arr[i++].Set( "MN", "MNG", 496 );		// MONGOLIA
		arr[i++].Set( "MS", "MSR", 500 );		// MONTSERRAT
		arr[i++].Set( "MA", "MAR", 504 );		// MOROCCO
		arr[i++].Set( "MZ", "MOZ", 508 );		// MOZAMBIQUE
		arr[i++].Set( "MM", "MMR", 104 );		// MYANMAR
		arr[i++].Set( "NA", "NAM", 516 );		// NAMIBIA
		arr[i++].Set( "NR", "NRU", 520 );		// NAURU
		arr[i++].Set( "NP", "NPL", 524 );		// NEPAL
		arr[i++].Set( "NL", "NLD", 528 );		// NETHERLANDS
		arr[i++].Set( "AN", "ANT", 530 );		// NETHERLANDS ANTILLES
		arr[i++].Set( "NC", "NCL", 540 );		// NEW CALEDONIA
		arr[i++].Set( "NZ", "NZL", 554 );		// NEW ZEALAND
		arr[i++].Set( "NI", "NIC", 558 );		// NICARAGUA
		arr[i++].Set( "NE", "NER", 562 );		// NIGER
		arr[i++].Set( "NG", "NGA", 566 );		// NIGERIA
		arr[i++].Set( "NU", "NIU", 570 );		// NIUE
		arr[i++].Set( "NF", "NFK", 574 );		// NORFOLK ISLAND
		arr[i++].Set( "MP", "MNP", 580 );		// NORTHERN MARIANA ISLANDS
		arr[i++].Set( "NO", "NOR", 578 );		// NORWAY
		arr[i++].Set( "OM", "OMN", 512 );		// OMAN
		arr[i++].Set( "PK", "PAK", 586 );		// PAKISTAN
		arr[i++].Set( "PW", "PLW", 585 );		// PALAU
		arr[i++].Set( "PA", "PAN", 591 );		// PANAMA
		arr[i++].Set( "PG", "PNG", 598 );		// PAPUA NEW GUINEA
		arr[i++].Set( "PY", "PRY", 600 );		// PARAGUAY
		arr[i++].Set( "PE", "PER", 604 );		// PERU
		arr[i++].Set( "PH", "PHL", 608 );		// PHILIPPINES
		arr[i++].Set( "PN", "PCN", 612 );		// PITCAIRN
		arr[i++].Set( "PL", "POL", 616 );		// POLAND
		arr[i++].Set( "PT", "PRT", 620 );		// PORTUGAL
		arr[i++].Set( "PR", "PRI", 630 );		// PUERTO RICO
		arr[i++].Set( "QA", "QAT", 634 );		// QATAR
		arr[i++].Set( "RE", "REU", 638 );		// REUNION
		arr[i++].Set( "RO", "ROM", 642 );		// ROMANIA
		arr[i++].Set( "RU", "RUS", 643 );		// RUSSIAN FEDERATION
		arr[i++].Set( "RW", "RWA", 646 );		// RWANDA
		arr[i++].Set( "KN", "KNA", 659 );		// SAINT KITTS AND NEVIS
		arr[i++].Set( "LC", "LCA", 662 );		// SAINT LUCIA 
		arr[i++].Set( "VC", "VCT", 670 );		// SAINT VINCENT AND THE GRENADINES
		arr[i++].Set( "WS", "WSM", 882 );		// SAMOA
		arr[i++].Set( "SM", "SMR", 674 );		// SAN MARINO
		arr[i++].Set( "ST", "STP", 678 );		// SAO TOME AND PRINCIPE
		arr[i++].Set( "SA", "SAU", 682 );		// SAUDI ARABIA
		arr[i++].Set( "SN", "SEN", 686 );		// SENEGAL
		arr[i++].Set( "SC", "SYC", 690 );		// SEYCHELLES
		arr[i++].Set( "SL", "SLE", 694 );		// SIERRA LEONE
		arr[i++].Set( "SG", "SGP", 702 );		// SINGAPORE
		arr[i++].Set( "SK", "SVK", 703 );		// SLOVAKIA (Slovak Republic)
		arr[i++].Set( "SI", "SVN", 705 );		// SLOVENIA
		arr[i++].Set( "SB", "SLB",  90 );		// SOLOMON ISLANDS
		arr[i++].Set( "SO", "SOM", 706 );		// SOMALIA
		arr[i++].Set( "ZA", "ZAF", 710 );		// SOUTH AFRICA
		arr[i++].Set( "GS", "SGS", 239 );		// SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS
		arr[i++].Set( "ES", "ESP", 724 );		// SPAIN
		arr[i++].Set( "LK", "LKA", 144 );		// SRI LANKA
		arr[i++].Set( "SH", "SHN", 654 );		// ST. HELENA
		arr[i++].Set( "PM", "SPM", 666 );		// ST. PIERRE AND MIQUELON
		arr[i++].Set( "SD", "SDN", 736 );		// SUDAN
		arr[i++].Set( "SR", "SUR", 740 );		// SURINAME
		arr[i++].Set( "SJ", "SJM", 744 );		// SVALBARD AND JAN MAYEN ISLANDS
		arr[i++].Set( "SZ", "SWZ", 748 );		// SWAZILAND
		arr[i++].Set( "SE", "SWE", 752 );		// SWEDEN
		arr[i++].Set( "CH", "CHE", 756 );		// SWITZERLAND
		arr[i++].Set( "SY", "SYR", 760 );		// SYRIAN ARAB REPUBLIC
		arr[i++].Set( "TW", "TWN", 158 );		// TAIWAN, PROVINCE OF CHINA
		arr[i++].Set( "TJ", "TJK", 762 );		// TAJIKISTAN
		arr[i++].Set( "TZ", "TZA", 834 );		// TANZANIA, UNITED REPUBLIC OF
		arr[i++].Set( "TH", "THA", 764 );		// THAILAND
		arr[i++].Set( "TG", "TGO", 768 );		// TOGO
		arr[i++].Set( "TK", "TKL", 772 );		// TOKELAU
		arr[i++].Set( "TO", "TON", 776 );		// TONGA
		arr[i++].Set( "TT", "TTO", 780 );		// TRINIDAD AND TOBAGO
		arr[i++].Set( "TN", "TUN", 788 );		// TUNISIA
		arr[i++].Set( "TR", "TUR", 792 );		// TURKEY
		arr[i++].Set( "TM", "TKM", 795 );		// TURKMENISTAN
		arr[i++].Set( "TC", "TCA", 796 );		// TURKS AND CAICOS ISLANDS
		arr[i++].Set( "TV", "TUV", 798 );		// TUVALU
		arr[i++].Set( "UG", "UGA", 800 );		// UGANDA
		arr[i++].Set( "UA", "UKR", 804 );		// UKRAINE
		arr[i++].Set( "AE", "ARE", 784 );		// UNITED ARAB EMIRATES	
		arr[i++].Set( "GB", "GBR", 826 );		// UNITED KINGDOM
		arr[i++].Set( "US", "USA", 840 );		// UNITED STATES
		arr[i++].Set( "UM", "UMI", 581 );		// UNITED STATES MINOR OUTLYING ISLANDS
		arr[i++].Set( "UY", "URY", 858 );		// URUGUAY
		arr[i++].Set( "UZ", "UZB", 860 );		// UZBEKISTAN
		arr[i++].Set( "VU", "VUT", 548 );		// VANUATU
		arr[i++].Set( "VE", "VEN", 862 );		// VENEZUELA
		arr[i++].Set( "VN", "VNM", 704 );		// VIET NAM
		arr[i++].Set( "VG", "VGB",  92 );		// VIRGIN ISLANDS (BRITISH)
		arr[i++].Set( "VI", "VIR", 850 );		// VIRGIN ISLANDS (U.S.)
		arr[i++].Set( "WF", "WLF", 876 );		// WALLIS AND FUTUNA ISLANDS
		arr[i++].Set( "EH", "ESH", 732 );		// WESTERN SAHARA
		arr[i++].Set( "YE", "YEM", 887 );		// YEMEN
		arr[i++].Set( "YU", "YUG", 891 );		// YUGOSLAVIA
		arr[i++].Set( "ZR", "ZAR", 180 );		// ZAIRE
		arr[i++].Set( "ZM", "ZMB", 894 );		// ZAMBIA
		arr[i++].Set( "ZW", "ZWE", 716 );		// ZIMBABWE

		ASSERT( i == arr.Count() );
	}

	
}	// Base
}	// Engine
