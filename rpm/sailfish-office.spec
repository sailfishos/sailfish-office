Name: sailfish-office
Version: 1.5.20
Release: 1
Summary: Sailfish office viewer
Group: Applications/Office
License: GPLv2
Source0: %{name}-%{version}.tar.gz
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Widgets)
#BuildRequires: pkgconfig(Qt5WebKit)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: pkgconfig(sailfishsilica) >= 1.1.8
BuildRequires: libqt5sparql-devel
BuildRequires: poppler-qt5-devel poppler-qt5 poppler-devel poppler
BuildRequires: mapplauncherd-qt5-devel
BuildRequires: cmake
BuildRequires: qt5-qttools-linguist
BuildRequires: pkgconfig(icu-i18n)
Requires: calligra-components
Requires: calligra-filters >= 3.1.0+git18
Requires: sailfishsilica-qt5 >= 1.1.63
Requires: sailfish-components-accounts-qt5
Requires: sailfish-components-textlinking
Requires: libqt5sparql-tracker
Requires: mapplauncherd >= 4.1.17
#Requires: qt5-qtqml-import-webkitplugin
Requires: nemo-qml-plugin-configuration-qt5
Requires:  %{name}-all-translations
Requires: sailfish-content-graphics
Requires: qt5-qtdeclarative-import-qtquick2plugin >= 5.4.0
Requires: declarative-transferengine-qt5 >= 0.3.1

%package ts-devel
Summary: Translation source for %{name}
License: GPLv2
Group: System/Base


%description
%{summary}.

%description ts-devel
%{summary}.


%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_libdir}/qt5/qml/Sailfish/Office/
%{_datadir}/applications/*.desktop
%{_datadir}/%{name}/
%{_datadir}/translations/*.qm
%{_datadir}/dbus-1/interfaces/
%{_datadir}/dbus-1/services/
%{_datadir}/mapplauncherd/privileges.d/sailfish-office.privileges

%files ts-devel
%{_datadir}/translations/source/*.ts


%prep
%setup -q -n %{name}-%{version}


%build
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?_smp_mflags}


%install
make DESTDIR=%{buildroot} install


%post
/usr/bin/update-desktop-database -q
