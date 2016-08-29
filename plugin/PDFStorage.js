/*
 * Copyright (C) 2015 Caliste Damien.
 * Contact: Damien Caliste <dcaliste@free.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

var Settings = function(file) {
    this.db = LocalStorage.openDatabaseSync("sailfish-office", "1.0",
                                            "Local storage for the document viewer.", 10000);
    this.source = file
}

/* Different tables. */
function createTableLastViewSettings(tx) {
    /* Currently store the last page, may be altered later to store
       zoom level or page position. */
    tx.executeSql("CREATE TABLE IF NOT EXISTS LastViewSettings("
                  + "file TEXT   NOT NULL,"
                  + "page INT    NOT NULL,"
                  + "top  REAL           ,"
                  + "left REAL           ,"
                  + "width INT CHECK(width > 0))");
    tx.executeSql('CREATE UNIQUE INDEX IF NOT EXISTS idx_file ON LastViewSettings(file)');
}

/* Get and set operations. */
Settings.prototype.getLastPage = function() {
    var page = 0
    var top = 0
    var left = 0
    var width = 0
    var file = this.source
    this.db.transaction(function(tx) {
        createTableLastViewSettings(tx);
        var rs = tx.executeSql('SELECT page, top, left, width FROM LastViewSettings WHERE file = ?', [file]);
        if (rs.rows.length > 0) {
            page = rs.rows.item(0).page;
            top  = rs.rows.item(0).top;
            left = rs.rows.item(0).left;
            width = rs.rows.item(0).width;
        }
    });
    // Return page is in [1:]
    return [page, top, left, width];
}
Settings.prototype.setLastPage = function(page, top, left, width) {
    // page is in [1:]
    var file = this.source
    this.db.transaction(function(tx) {
        createTableLastViewSettings(tx);
        var rs = tx.executeSql('INSERT OR REPLACE INTO LastViewSettings(file, page, top, left, width) VALUES (?,?,?,?,?)', [file, page, top, left, width]);
    });
}
