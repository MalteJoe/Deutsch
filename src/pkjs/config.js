module.exports = [
  {
    type: "heading",
    defaultValue: "Einstellungen",
  },
  {
    type: "section",
    items: [
      {
        type: "heading",
        defaultValue: "Anzeige",
      },
      {
        type: "select",
        messageKey: "ColorTheme",
        defaultValue: "0",
        label: "Farbschema",
        serializeValueAs: "integer",
        options: [
          {
            label: "Schwarz",
            value: "0",
          },
          {
            label: "Blau",
            value: "1",
          },
          {
            label: "Grün",
            value: "2",
          },
          {
            label: "Rot",
            value: "3",
          },
          {
            label: "Grau",
            value: "4",
          },
          {
            label: "Weiß",
            value: "5",
          },
        ],
      },
      {
        type: "select",
        messageKey: "TextAlignment",
        defaultValue: "2",
        label: "Textausrichtung",
        capabilities: ["RECT"],
        serializeValueAs: "integer",
        options: [
          {
            label: "Links",
            value: "0",
          },
          {
            label: "Mittig",
            value: "1",
          },
          {
            label: "Rechts",
            value: "2",
          },
        ],
      },
    ],
  },
  {
    type: "section",
    items: [
      {
        type: "heading",
        defaultValue: "Uhrzeit",
      },
      {
        type: "toggle",
        messageKey: "Fuzzy",
        label: "Uhrzeit runden",
        description: "z.B. „kurz vor acht“ statt „zwei vor acht“",
        defaultValue: true,
      },
      {
        type: "select",
        messageKey: "TextWien",
        label: "10:15 ist…",
        defaultValue: "0",
        serializeValueAs: "integer",
        options: [
          {
            label: "Viertel nach zehn",
            value: "0",
          },
          {
            label: "Viertel elf",
            value: "1",
          },
        ],
      },
      {
        type: "select",
        messageKey: "TextNRW",
        label: "10:45 ist…",
        defaultValue: "1",
        serializeValueAs: "integer",
        options: [
          {
            label: "Viertel vor elf",
            value: "1",
          },
          {
            label: "Dreiviertel elf",
            value: "0",
          },
        ],
      },
      {
        type: "toggle",
        messageKey: "Date",
        label: "Datum anzeigen",
        defaultValue: true,
      },
    ],
  },
  {
    type: "section",
    items: [
      {
        type: "heading",
        defaultValue: "Status"
      },
      {
        type: "select",
        messageKey: "BatteryIcon",
        label: "Batteriestatus anzeigen",
        defaultValue: "1",
        serializeValueAs: "integer",
        options: [
          {
            "label": "Nie",
            "value": "0"
          },
          {
            "label": "Wenn unter 10%",
            "value": "1"
          },
          {
            "label": "Immer",
            "value": "2"
          },
        ]
      },
      {
        type: "select",
        messageKey: "BluetoothIcon",
        label: "Bluetooth-Icon anzeigen",
        defaultValue: "1",
        serializeValueAs: "integer",
        options: [
          {
            "label": "Nie",
            "value": "0"
          },
          {
            "label": "Wenn nicht verbunden",
            "value": "1"
          },
          {
            "label": "Immer",
            "value": "2"
          },
        ]
      },
      {
        type: "toggle",
        messageKey: "VibeOnDisconnect",
        label: "Vibrieren bei Bluetooth-Verlust",
        defaultValue: true,
      },
    ]
  },
  {
    type: "submit",
    defaultValue: "Speichern",
  },
];
