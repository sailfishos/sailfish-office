/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    License is under GPLv2 <http://www.gnu.org/licenses/gpl-2.0.txt>

*/

#include "folderitem.h"
#include <QStringList>

QHash<int, QByteArray> FolderItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[RevisionRole] = "revision";
  names[Thumb_existsRole] = "thumb_exists";
  names[BytesRole] = "bytes";
  names[ModifiedRole] = "modified";
  names[PathRole] = "path";
  names[Is_dirRole] = "is_dir";
  names[IconRole] = "icon";
  names[Mime_typeRole] = "mime_type";
  names[SizeRole] = "size";
  names[CheckedRole] = "checked";
  names[NameRole] = "name";
  names[SectionRole] = "section";
  return names;
}

QVariant FolderItem::data(int role) const
{
  switch(role) {
  case RevisionRole: return revision();break;
  case Thumb_existsRole: return thumb_exists();break;
  case BytesRole: return bytes();break;
  case ModifiedRole: return modified();break;
  case PathRole: return path();break;
  case Is_dirRole: return is_dir();break;
  case IconRole: return icon();break;
  case Mime_typeRole: return mime_type();break;
  case SizeRole: return size();break;
  case CheckedRole: return checked();break;
  case NameRole: return name();break;
  case SectionRole: return section();break;
    default:
        return QVariant();
  }
}

QString FolderItem::name() const {
    return "";
}

QString FolderItem::section() const {
    QStringList ala(path().split("/"));
    QString lab(ala.at(ala.length()-1));
    return lab.at(0);
}

QString FolderItem::xsection() {
    return section();
}
