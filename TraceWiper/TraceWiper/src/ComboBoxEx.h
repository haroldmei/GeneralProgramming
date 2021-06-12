//////////////////////////////////////////////////////
// ComboBoxEx.h

#ifndef COMBOBOXEX_H
#define COMBOBOXEX_H



// Declaration of the CComboBoxEx_Win32xx class
class CComboBoxEx_Win32xx : public CWnd
{
public:
	CComboBoxEx_Win32xx();
	virtual ~CComboBoxEx_Win32xx();
	BOOL AddItems();
	void SetImages(int nImages, UINT ImageID);

protected:
	virtual void PreCreate(CREATESTRUCT &cs);

private:
	HIMAGELIST m_himlImages;
};


#endif // COMBOBOXEX_H
