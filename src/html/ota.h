/*
 * ota.h
 * Generated from ota.html
 */

#ifndef HTML_OTA_H
#define HTML_OTA_H

#include <string>

const char html_ota[] = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" /><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>LeafMiner OTA</title><style>      body {       font-family: Arial, sans-serif;        background-color: #f4f4f4;        margin: 0;        padding: 20px;     }      form {       max-width: 400px;        margin: 0 auto;        background-color: #fff;        padding: 20px;        border-radius: 8px;        box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);     }      p {       width: auto;        text-align: center;      }      label {       display: block;        margin-bottom: 8px;     }      input {       width: 100%;        padding: 8px;        margin-bottom: 16px;        box-sizing: border-box;        border: 1px solid #ccc;        border-radius: 4px;     }      input[type=\"radio\"] {       width: auto;     }      input[type=\"submit\"] {       background-color: #4caf50;        color: #fff;        cursor: pointer;     }      input[type=\"submit\"]:hover {       background-color: #45a049;     }    </style></head><body><form method=\"post\" action=\"/upload\" enctype=\"multipart/form-data\"><h1>LeafMiner</h1><label>OTA bin file:</label><input type=\"file\" name=\"update\"><br /><input type=\"submit\" value=\"Upload & Install\" /></form></body></html>";

#endif // HTML_OTA_H
