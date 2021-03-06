// JPEGsnoop - JPEG Image Decoder & Analysis Utility
// Copyright (C) 2017 - Calvin Hass
// http://www.impulseadventure.com/photo/jpeg-snoop.html
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// ====================================================================================================
// SOURCE CODE ACKNOWLEDGEMENT
// ====================================================================================================
// The following code is derived from the following project on CodeProject:
//
//      Title:      Another registry class
//      Author:     SteveKing
//      URL:        http://www.codeproject.com/Articles/2521/Another-registry-class
//      Date:       Apr 25, 2003
//      License:    CPOL (Code Project Open License)
//
// ====================================================================================================

#include "stdafx.h"

#include "Registry.h"
#include <vector>

#ifdef _MFC_VER
//MFC is available - also use the MFC-based classes

CRegDWORD::CRegDWORD() :
    m_value(0),
    m_defaultvalue(0),
    m_read(false),
    m_force(false)
{
    m_base = HKEY_CURRENT_USER;
}

/**
 * Constructor.
 * @param key the path to the key, including the key. example: "Software\\Company\\SubKey\\MyValue"
 * @param def the default value used when the key does not exist or a read error occured
 * @param force set to true if no cache should be used, i.e. always read and write directly from/to registry
 * @param base a predefined base key like HKEY_LOCAL_MACHINE. see the SDK documentation for more information.
 */
CRegDWORD::CRegDWORD(CString key, DWORD def, bool force, HKEY base)
{
    m_value = 0;
    m_defaultvalue = def;
    m_force = force;
    m_base = base;
    m_read = false;
    key.TrimLeft(_T("\\"));
    m_path = key.Left(key.ReverseFind(_T('\\')));
    m_path.TrimRight(_T("\\"));
    m_key = key.Right(key.GetLength() - key.ReverseFind(_T('\\')));
    m_key.Trim(_T("\\"));
    read();
}

DWORD CRegDWORD::read()
{
    ASSERT(m_key != _T(""));
    if (RegOpenKeyEx(m_base, m_path, 0, KEY_EXECUTE, &m_hKey) == ERROR_SUCCESS)
    {
        DWORD size = sizeof(m_value);
        DWORD type;
        if (RegQueryValueEx(m_hKey, m_key, nullptr, &type, reinterpret_cast<BYTE*>(&m_value), &size) == ERROR_SUCCESS)
        {
            ASSERT(type==REG_DWORD);
            m_read = true;
            RegCloseKey(m_hKey);
            return m_value;
        }
        RegCloseKey(m_hKey);
        return m_defaultvalue;
    }
    return m_defaultvalue;
}

void CRegDWORD::write()
{
    ASSERT(!m_key.IsEmpty());
    DWORD disp;
    if (RegCreateKeyEx(m_base, m_path, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &m_hKey, &disp) != ERROR_SUCCESS)
    {
        return;
    }
    if (RegSetValueEx(m_hKey, m_key, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&m_value), sizeof(m_value)) == ERROR_SUCCESS)
    {
        m_read = true;
    }
    RegCloseKey(m_hKey);
}


CRegDWORD::operator DWORD()
{
    if ((m_read) && (!m_force))
        return m_value;
    return read();
}

CRegDWORD& CRegDWORD::operator =(DWORD d)
{
    if ((d == m_value) && (!m_force))
    {
        //no write to the registry required, its the same value
        return *this;
    }
    m_value = d;
    write();
    return *this;
}

CRegString::CRegString()
{
    m_base = HKEY_CURRENT_USER;
    m_read = false;
    m_force = false;
}

/**
 * Constructor.
 * @param key the path to the key, including the key. example: "Software\\Company\\SubKey\\MyValue"
 * @param def the default value used when the key does not exist or a read error occured
 * @param force set to true if no cache should be used, i.e. always read and write directly from/to registry
 * @param base a predefined base key like HKEY_LOCAL_MACHINE. see the SDK documentation for more information.
 */
CRegString::CRegString(CString key, CString def, bool force, HKEY base)
{
    m_defaultvalue = def;
    m_force = force;
    m_base = base;
    m_read = false;
    key.TrimLeft(_T("\\"));
    m_path = key.Left(key.ReverseFind(_T('\\')));
    m_path.TrimRight(_T("\\"));
    m_key = key.Right(key.GetLength() - key.ReverseFind(_T('\\')));
    m_key.Trim(_T("\\"));
    read();
}

CString CRegString::read()
{
    ASSERT(!m_key.IsEmpty());
    if (RegOpenKeyEx(m_base, m_path, 0, KEY_EXECUTE, &m_hKey) == ERROR_SUCCESS)
    {
        DWORD size = 0;
        DWORD type;
        RegQueryValueEx(m_hKey, m_key, nullptr, &type, nullptr, &size);
        std::vector<TCHAR> pStr(size);
        if (RegQueryValueEx(m_hKey, m_key, nullptr, &type, reinterpret_cast<BYTE*>(pStr.data()), &size) == ERROR_SUCCESS)
        {
            m_value = CString(pStr.data());
            ASSERT(type==REG_SZ);
            m_read = true;
            RegCloseKey(m_hKey);
            return m_value;
        }
        RegCloseKey(m_hKey);
        return m_defaultvalue;
    }
    return m_defaultvalue;
}

void CRegString::write()
{
    ASSERT(!m_key.IsEmpty());
    DWORD disp;
    if (RegCreateKeyEx(m_base, m_path, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &m_hKey, &disp) != ERROR_SUCCESS)
    {
        return;
    }
    if (RegSetValueEx(m_hKey, m_key, 0, REG_SZ, reinterpret_cast<const BYTE*>(m_value.GetString()), (m_value.GetLength() + 1) * sizeof(TCHAR)) == ERROR_SUCCESS)
    {
        m_read = true;
    }
    RegCloseKey(m_hKey);
}

CRegString::operator CString()
{
    if ((m_read) && (!m_force))
        return m_value;
    return read();
}

CRegString& CRegString::operator =(CString s)
{
    if ((s == m_value) && (!m_force))
    {
        //no write to the registry required, its the same value
        return *this;
    }
    m_value = s;
    write();
    return *this;
}

#endif

CRegStdString::CRegStdString()
{
    m_base = HKEY_CURRENT_USER;
    m_read = false;
    m_force = false;
}

/**
 * Constructor.
 * @param key the path to the key, including the key. example: "Software\\Company\\SubKey\\MyValue"
 * @param def the default value used when the key does not exist or a read error occured
 * @param force set to true if no cache should be used, i.e. always read and write directly from/to registry
 * @param base a predefined base key like HKEY_LOCAL_MACHINE. see the SDK documentation for more information.
 */
CRegStdString::CRegStdString(std::wstring key, std::wstring def, BOOL force, HKEY base)
{
    m_defaultvalue = def;
    m_force = force;
    m_base = base;
    m_read = false;

    std::wstring::size_type pos = key.find_last_of(_T('\\'));
    m_path = key.substr(0, pos);
    m_key = key.substr(pos + 1);
    read();
}

std::wstring CRegStdString::read()
{
    if (RegOpenKeyEx(m_base, m_path.c_str(), 0, KEY_EXECUTE, &m_hKey) == ERROR_SUCCESS)
    {
        DWORD size = 0;
        DWORD type;
        RegQueryValueEx(m_hKey, m_key.c_str(), nullptr, &type, nullptr, &size);
        std::vector<TCHAR> pStr(size);
        if (RegQueryValueEx(m_hKey, m_key.c_str(), nullptr, &type, reinterpret_cast<BYTE*>(pStr.data()), &size) == ERROR_SUCCESS)
        {
            m_value.assign(pStr.data());
            m_read = true;
            RegCloseKey(m_hKey);
            return m_value;
        }
        RegCloseKey(m_hKey);
        return m_defaultvalue;
    }
    return m_defaultvalue;
}

void CRegStdString::write()
{
    DWORD disp;
    if (RegCreateKeyEx(m_base, m_path.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &m_hKey, &disp) != ERROR_SUCCESS)
    {
        return;
    }
    if (RegSetValueEx(m_hKey, m_key.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(m_value.c_str()), m_value.size() + 1) == ERROR_SUCCESS)
    {
        m_read = true;
    }
    RegCloseKey(m_hKey);
}

CRegStdString::operator LPCTSTR()
{
    if ((m_read) && (!m_force))
        return m_value.c_str();
    return read().c_str();
}

CRegStdString::operator std::wstring()
{
    if ((m_read) && (!m_force))
        return m_value;
    return read();
}

CRegStdString& CRegStdString::operator =(std::wstring s)
{
    if ((s == m_value) && (!m_force))
    {
        //no write to the registry required, its the same value
        return *this;
    }
    m_value = s;
    write();
    return *this;
}

CRegStdWORD::CRegStdWORD()
{
    m_value = 0;
    m_defaultvalue = 0;
    m_base = HKEY_CURRENT_USER;
    m_read = false;
    m_force = false;
}

/**
 * Constructor.
 * @param key the path to the key, including the key. example: "Software\\Company\\SubKey\\MyValue"
 * @param def the default value used when the key does not exist or a read error occured
 * @param force set to true if no cache should be used, i.e. always read and write directly from/to registry
 * @param base a predefined base key like HKEY_LOCAL_MACHINE. see the SDK documentation for more information.
 */
CRegStdWORD::CRegStdWORD(std::wstring key, DWORD def, BOOL force, HKEY base)
{
    m_value = 0;
    m_defaultvalue = def;
    m_force = force;
    m_base = base;
    m_read = false;

    std::wstring::size_type pos = key.find_last_of(_T('\\'));
    m_path = key.substr(0, pos);
    m_key = key.substr(pos + 1);
    read();
}

DWORD CRegStdWORD::read()
{
    if (RegOpenKeyEx(m_base, m_path.c_str(), 0, KEY_EXECUTE, &m_hKey) == ERROR_SUCCESS)
    {
        DWORD size = sizeof(m_value);
        DWORD type;
        if (RegQueryValueEx(m_hKey, m_key.c_str(), nullptr, &type, reinterpret_cast<BYTE*>(&m_value), &size) == ERROR_SUCCESS)
        {
            m_read = true;
            RegCloseKey(m_hKey);
            return m_value;
        }
        RegCloseKey(m_hKey);
        return m_defaultvalue;
    }
    return m_defaultvalue;
}

void CRegStdWORD::write()
{
    DWORD disp;
    if (RegCreateKeyEx(m_base, m_path.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &m_hKey, &disp) != ERROR_SUCCESS)
    {
        return;
    }
    if (RegSetValueEx(m_hKey, m_key.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&m_value), sizeof(m_value)) == ERROR_SUCCESS)
    {
        m_read = true;
    }
    RegCloseKey(m_hKey);
}

CRegStdWORD::operator DWORD()
{
    if ((m_read) && (!m_force))
        return m_value;
    return read();
}
