///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/aboutdlgg.cpp
// Purpose:     implements wxGenericAboutBox() function
// Author:      Vadim Zeitlin
// Created:     2006-10-08
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_ABOUTDLG

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
    #include "wx/statbmp.h"
    #include "wx/stattext.h"
    #include "wx/button.h"
#endif //WX_PRECOMP

#include "wx/aboutdlg.h"
#include "wx/generic/aboutdlgg.h"

#include "wx/hyperlink.h"
#include "wx/collpane.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxAboutDialogInfo
// ----------------------------------------------------------------------------

// helper function: returns all array elements in a single comma-separated and
// newline-terminated string
static wxString AllAsString(const wxArrayString& a)
{
    wxString s;
    const size_t count = a.size();
    for ( size_t n = 0; n < count; n++ )
    {
        s << a[n] << (n == count - 1 ? _T("\n") : _T(", "));
    }

    return s;
}

wxString wxAboutDialogInfo::GetDescriptionAndCredits() const
{
    wxString s = GetDescription();
    if ( !s.empty() )
        s << _T('\n');

    if ( HasDevelopers() )
        s << _T('\n') << _("Developed by ") << AllAsString(GetDevelopers());

    if ( HasDocWriters() )
        s << _T('\n') << _("Documentation by ") << AllAsString(GetDocWriters());

    if ( HasArtists() )
        s << _T('\n') << _("Graphics art by ") << AllAsString(GetArtists());

    if ( HasTranslators() )
        s << _T('\n') << _("Translations by ") << AllAsString(GetTranslators());

    return s;
}

wxIcon wxAboutDialogInfo::GetIcon() const
{
    wxIcon icon = m_icon;
    if ( !icon.Ok() && wxTheApp )
    {
        const wxTopLevelWindow * const
            tlw = wxDynamicCast(wxTheApp->GetTopWindow(), wxTopLevelWindow);
        if ( tlw )
            icon = tlw->GetIcon();
    }

    return icon;
}

// ----------------------------------------------------------------------------
// wxGenericAboutDialog
// ----------------------------------------------------------------------------

bool wxGenericAboutDialog::Create(const wxAboutDialogInfo& info)
{
    // TODO: should we use main frame as parent by default here?
    if ( !wxDialog::Create(NULL, wxID_ANY, _("About ") + info.GetName(),
                           wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE) )
        return false;

    m_sizerText = new wxBoxSizer(wxVERTICAL);
    wxString nameAndVersion = info.GetName();
    if ( info.HasVersion() )
        nameAndVersion << _T(' ') << info.GetVersion();
    wxStaticText *label = new wxStaticText(this, wxID_ANY, nameAndVersion);
    wxFont fontBig(*wxNORMAL_FONT);
    fontBig.SetPointSize(fontBig.GetPointSize() + 2);
    fontBig.SetWeight(wxFONTWEIGHT_BOLD);
    label->SetFont(fontBig);

    m_sizerText->Add(label, wxSizerFlags().Centre().Border());
    m_sizerText->AddSpacer(5);

    AddText(info.GetCopyright());
    AddText(info.GetDescription());

    if ( info.HasWebSite() )
    {
#if wxUSE_HYPERLINKCTRL
        AddControl(new wxHyperlinkCtrl(this, wxID_ANY,
                                       info.GetWebSiteDescription(),
                                       info.GetWebSiteURL()));
#else
        AddText(info.GetWebSiteURL());
#endif // wxUSE_HYPERLINKCTRL/!wxUSE_HYPERLINKCTRL
    }

#if wxUSE_COLLPANE
    // add licence
    if ( info.HasLicence() )
    {
        wxCollapsiblePane *
            licensepnl = new wxCollapsiblePane(this, wxID_ANY, wxT("License"));

        new wxStaticText(licensepnl->GetPane(), wxID_ANY, info.GetLicence(),
                         wxDefaultPosition, wxDefaultSize,
                         wxALIGN_CENTRE);

        m_sizerText->Add(licensepnl, wxSizerFlags(1).Expand().Border(wxBOTTOM));
    }
#endif // wxUSE_COLLPANE

    // TODO: add credits (developers, artists, doc writers, translators)

    DoAddCustomControls();


    wxSizer *sizerIconAndText = new wxBoxSizer(wxHORIZONTAL);
#if wxUSE_STATBMP
    wxIcon icon = info.GetIcon();
    if ( icon.Ok() )
    {
        sizerIconAndText->Add(new wxStaticBitmap(this, wxID_ANY, icon),
                                wxSizerFlags().Border(wxRIGHT));
    }
#endif // wxUSE_STATBMP
    sizerIconAndText->Add(m_sizerText, wxSizerFlags(1).Expand());

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(sizerIconAndText, wxSizerFlags(1).Expand().Border());

    wxSizer *sizerBtns = CreateButtonSizer(wxOK);
    if ( sizerBtns )
    {
        sizerTop->Add(sizerBtns, wxSizerFlags().Expand().Border());
    }

    SetSizerAndFit(sizerTop);

    CentreOnScreen();

    return true;
}

void wxGenericAboutDialog::AddControl(wxWindow *win, const wxSizerFlags& flags)
{
    wxCHECK_RET( m_sizerText, _T("can only be called after Create()") );
    wxASSERT_MSG( win, _T("can't add NULL window to about dialog") );

    m_sizerText->Add(win, flags);
}

void wxGenericAboutDialog::AddControl(wxWindow *win)
{
    AddControl(win, wxSizerFlags().Border(wxDOWN).Centre());
}

void wxGenericAboutDialog::AddText(const wxString& text)
{
    if ( !text.empty() )
        AddControl(new wxStaticText(this, wxID_ANY, text));
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

void wxGenericAboutBox(const wxAboutDialogInfo& info)
{
    wxGenericAboutDialog dlg(info);
    dlg.ShowModal();
}

// currently wxAboutBox is implemented natively only under these platforms, for
// the others we provide a generic fallback here
#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK26__)

void wxAboutBox(const wxAboutDialogInfo& info)
{
    wxGenericAboutBox(info);
}

#endif // platforms without native about dialog


#endif // wxUSE_ABOUTDLG
