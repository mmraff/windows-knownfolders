<svg width="0" height="0" xmlns="http://www.w3.org/2000/svg">
  <symbol id="extLinkSym" viewBox="0 0 20 16">
    <path stroke-width="2" stroke-linejoin="round" stroke-linecap="round"
          fill="none"
          d="M9,4 h-5 v11 h11 v-5 M8,11 L18,1 M12,1 h6 v6"></path>
  </symbol>
</svg>

# windows-knownfolders
Native addon for Node.js to query the Windows Known Folders API

## Install
<pre style="color:#ccc;background-color:#222;">
C:\Users\myUser><b>npm install windows-knownfolders</b>
</pre>
## 

## Usage
```js
var knownfolders = require('windows-knownfolders')

// To get the definitive answer to: Where is my home directory?
// (as far as that can be stated when on Windows)
var profileId = knownfolders.folderIds.Profile
knownfolders.getPath(profileId, function(er, profilePath) {
  // Do something with profilePath...
})

// Request an enumeration of Ids for all Known Folders currently on this system:
knownfolders.list(function(er, data) {
  // data is an array of Ids; each can be supplied to other calls...
})

// Maybe we know the name, but we need to discover the Id:
knownfolders.getByName('OneDrive', function(er, kfid) {
  console.log(knownfolders.getIdName(kfid)) // SkyDrive

  // To get the properties of this folder for inspection:
  knownfolders.getDefinition(kfid, function(er, data) {
    // Do something with data...
  })
})
```
## 

## API

### General Notes
In the following,
* *module-designated Id* means a unique number generated by the module.
* A result of 0 for a function that fetches an Id means that no match was found.
* No attempt is made to expose the GUIDs used by the underlying system libraries.
* Where a user-supplied Id is not recognized, it means it has not been obtained
  from the module.

#### The Internal List of Ids
*Statement A:* Some of the functions have a side effect: when a previously
unrecognized Id is discovered by a query, the module remembers it for the rest
of the lifetime of the instance by adding it to its dynamic internal list
(*not* in **folderIds**).
This side effect is noted in the descriptions of the applicable functions below.

*Statement B:* Where an asynchronous function takes an Id argument, the
function will immediately throw an error if both of the following are true:
* The Id is 0, or not from the set of **folderIds** constants (see below).
* The Id is not from the results of a previous query through the same runtime
instance of the module.

Taking the two statements above into account, it is possible that a call like
the following that throws an error at an early point in the life of a runtime
instance will neither throw nor pass back an error but fetch valid data at a
later point in the same runtime instance:
```js
// Where id is greater than the highest value in knownfolders.folderIds:
knownfolders.getDefinition(id, function(err, defData) {
  // ...
});
```

#### Folder Names
There are several kinds of names associated with Known Folders:
* Canonical name
* Display name
* the last component of the filesystem path
* the symbolic name of the folder Id

*(Not part of this discussion: Parsing name and Localized name.)*

For a given folder, all of the above can be different, so it is wise not to
assume them to be interchangeable, even though in some cases they may be the
same.

#### Mutating/Extending?
The current version of this module provides operations that fetch information,
but no operations to modify the Known Folders system.

[Known Folders Developer's Guide at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/shell/known-folders)

## 
### Functions

### `getIdName(id)`
*Synchronous.* Returns the module-designated symbolic name of the Known Folder
associated with the given Id, if any, or an empty string for a Known Folder
registered by third-party software. If the given Id is out of the currently
recognized range, an error is thrown.
- `id` {Number} The module-designated Id.

*Note: Given an existing Known Folder, if there is no corresponding symbolic
name in the SDK header files used to build the module, there will be no
module-designated name; however, there will be no error thrown if the passed Id
has been returned by a previous query. In this case, the return value will be
an empty string.*

[List of KNOWNFOLDERID constants (not up-to-date!) at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/shell/knownfolderid)

### `getIdTypeName(typeId)`
*Synchronous.* Returns the module-designated type name of the Known Folder Type
associated with the given Type Id, if any; otherwise an error is thrown.
- `typeId` {Number} The module-designated Type Id. See **folderTypeIds** below.

*Note: the module-designated type name is derived from the symbolic name
assigned to the FOLDERTYPEID by Microsoft.*

[List of FOLDERTYPEID constants at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/shell/foldertypeid)

### `findFromPath(pathName, callback)`
*Asynchronous.* Fetches the module-designated Id of the Known Folder if one is
registered for the given file system path; otherwise 0.

*A call to this function may cause the dynamic internal list of Ids to grow.*
- `pathName` {String} The absolute path of the folder.
- `callback` {Function}
  * **error** {Error | `null`}
  * **id** {Number} The module-designated Id or 0 (unknown), if no error.

*Notes:*
- *The underlying system call is hard-coded to search for an exact match
for the given pathname.*
- *More than one Known Folder Id can point to the same path.*

[API doc for IKnownFolderManager::FindFolderFromPath at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iknownfoldermanager-findfolderfrompath)

### `getByName(name, callback)`
*Asynchronous.* Fetches the module-designated Id of the Known Folder if one is
registered with the given name; otherwise 0.

*A call to this function may cause the dynamic internal list of Ids to grow.*
- `name` {String} The "non-localized, canonical name" of the folder.
- `callback` {Function}
  * **error** {Error | `null`}
  * **id** {Number} The module-designated Id or 0 (unknown), if no error.

[API doc for IKnownFolderManager::GetFolderByName at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iknownfoldermanager-getfolderbyname)

### `getCanonicalName(id, callback)`
*Asynchronous.* Fetches the "non-localized, canonical name" from the
[KNOWNFOLDER_DEFINITION
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/ns-shobjidl_core-knownfolder_definition)
retrieved for the given Id. If the given Id is out of the currently recognized
range, an error is thrown.
- `id` {Number} The module-designated Id.
- `callback` {Function}
  * **error** {Error | `null`}
  * **name** {String} The canonical name, if error is `null`.

### `getCategory(id, callback)`
*Asynchronous.* Fetches the category of the folder with the given Id.
If the given Id is out of the currently recognized range, an error is thrown.
- `id` {Number} The module-designated Id.
- `callback` {Function}
  * **error** {Error | `null`}
  * **category** {String} one of "virtual", "fixed", "common", or "peruser", 
    if error is `null`.

[API doc for IKnownFolder::GetCategory at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iknownfolder-getcategory)

### `getDefinition(id, callback)`
*Asynchronous.* Fetches the data that defines the Known Folder identified by the
given Id. If the given Id is out of the currently recognized range, an error is
thrown.

*A call to this function may cause the dynamic internal list of Ids to grow.*
- `id` {Number} The module-designated Id.
- `callback` {Function}
  * **error** {Error | `null`}
  * **defData** {Object} The definition data, if error is `null`.

The properties of the object passed through the callback map one-to-one with
those of the KNOWNFOLDER_DEFINITION structure ([API doc at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/ns-shobjidl_core-knownfolder_definition)
):

| Property      |  Type   | can be<br>`null`? | Description |
| ------------- | ------- | :----: | ----------- |
| fid           | number  |        | The module-designated Known Folder Id |
| category      | string  |        | One of "virtual", "fixed", "common", or "peruser" |
| name          | string  |        | The non-localized, canonical name; not to be confused with the display name |
| description   | string  |  yes   | A short description; *should* include the folder's purpose and usage |
| fidParent     | number  |        | Module-designated Id of the parent Known Folder, or `0` |
| relativePath  | string  |  yes   | Path relative to the parent folder specified in (non-zero) fidParent |
| parsingName   | string  |  yes   | The Shell namespace folder path of the folder |
| tooltip       | string  |  yes   | Default tooltip resource, of the form `MODULE_NAME,RESOURCE_ID` |
| localizedName | string  |  yes   | Default localized name resource, of the form `MODULE_NAME,RESOURCE_ID` |
| icon          | string  |  yes   | Default icon resource, of the form `MODULE_NAME,RESOURCE_ID` |
| security      | string  |  yes   | Default security descriptor, in [Security Descriptor Definition Language <svg width="20" height="16"><use href="#extLinkSym" stroke="#ccc"></use></svg>](https://docs.microsoft.com/windows/desktop/SecAuthZ/security-descriptor-definition-language) format |
| attributes    | number  |        | Default file system attributes; see the dwFlagsAndAttributes parameter of the [CreateFile function at microsoft.com<svg width="20" height="16"><use href="#extLinkSym" stroke="#ccc"></use></svg>](https://docs.microsoft.com/windows/desktop/api/fileapi/nf-fileapi-createfilea) |
| kfDefFlags    | number  |        | One of more values that control certain Known Folder behaviors; see **definitionFlags** below |
| folderTypeId  | number  |        | The module-designated Known Folder Type Id; see **folderTypeIds** below |

[API doc for IKnownFolder::GetFolderDefinition at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iknownfolder-getfolderdefinition)

### `getDisplayName(id, callback)`
*Asynchronous.* Fetches the display name of the shell item associated with the
Known Folder referenced by the given Id. If the Known Folder exists but has no
display name, the result is `null`.
If the given Id is out of the currently recognized range, an error is thrown.
- `id` {Number} The module-designated Id.
- `callback` {Function}
  * **error** {Error | `null`}
  * **name** {String} The display name, if error is `null`.

*Note: display names are not required to be unique.*

[API doc for IShellItem::GetDisplayName at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellitem-getdisplayname)

### `getFolderType(id, callback)`
*Asynchronous.* Fetches the Folder Type Id of the folder with the given Id.
If the given Id is out of the currently recognized range, an error is thrown.
- `id` {Number} The module-designated Id.
- `callback` {Function}
  * **error** {Error | `null`}
  * **typeId** {Number} one of the values in **folderTypeIds** (see below), 
    if error is `null`.

[API doc for IKnownFolder::GetFolderType at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iknownfolder-getfoldertype)

### `getIdList(id[, options], callback)`
*Asynchronous.* Fetches the binary data that uniquely identifies the Known Folder
in the Shell namespace. If the given Id is out of the currently recognized
range, an error is thrown.


*Not to be confused with fetching a list of Known Folders! For that, see **list**.*

For the full explanation, it would be best to refer to
[this document at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/shell/namespace-intro)
.
- `id` {Number} The module-designated Id.
- `options` {Number} *Optional* Any bitwise OR-combination of **pathOptions** values
 (see below) to specify special retrieval.
- `callback` {Function}
  * **error** {Error | `null`}
  * **bytes** {Buffer} a copy of the Shell Item ID List data, if error is `null`.

[API doc for SHGetKnownFolderIDList function at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderidlist)

### `getPath(id[, options], callback)`
Fetches the file system path, if any, of the Known Folder with the given
module-designated Id.
If the given Id is out of the currently recognized range, an error is thrown.
- `id` {Number} The module-designated Id.
- `options` {Number} *Optional* Any bitwise OR-combination of **pathOptions** values
 (see below) to specify special retrieval
- `callback` {Function}
  * **error** {Error | `null`}
  * **path** {String} The file system path, if error is `null`.

*Note: Some Known Folders do not have a file system path, even when present on
the system (e.g., virtual folders).*

[API doc for SHGetKnownFolderPath function at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath)

### `getRedirectionCapabilities(id, callback)`
*Asynchronous.* Fetches a value that combines the redirection capability flags
currently set on the Known Folder identified by the given Id. If the given Id
is out of the currently recognized range, an error is thrown.
- `id` {Number} The module-designated Id.
- `callback` {Function}
  * **error** {Error | `null`}
  * **flags** {Number} The flags combination value, if error is `null`.

See **redirectFlags** below for flag values to test for.

[API doc for IKnownFolder::GetRedirectionCapabilities at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iknownfolder-getredirectioncapabilities)

### `list([kind,] callback)`
*Asynchronous.* Fetches information of user-selectable kind for all Known Folders
registered on the system.

*A call to this function may cause the dynamic internal list of Ids to grow.*
- `kind` {Number} *Optional* Any one of **listTypes** values. If omitted,
 only Ids are fetched.
- `callback` {Function}
  * **error** {Error | `null`}
  * **data** {Array} The list, if error is `null`.

If `kind` is `listTypes.FOLDER_ID`, the result is the same as when no `kind` is
given.

If `kind` is `listTypes.DETAILS`, each element of the result list is of the
same form as described for **getDefinition**.

If `kind` is either `listTypes.CANONICAL_NAME` or `listTypes.DISPLAY_NAME`,
each element of the result list will be an object with two properties:
* `fid` {Number} The module-designated Id
* `name` {String} The (requested type of) name

*Warning: use of `listTypes.DISPLAY_NAME` as `kind` is relatively expensive.*

[API doc for IKnownFolderManager::GetFolderIds at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iknownfoldermanager-getfolderids)


# 

### Constants

### `folderIds`
Constants that correspond to FOLDERID values *available* on the current system
according to the installed SDK headers. Not all are guaranteed to have
corresponding folders *existent* on the current system.

Notes:
* It is very important to understand that this is *not* a list of the Known
  Folders that exist on the current system; it only references the Known Folder
  Ids declared in the SDK header files that are used when the module is built.
* It is possible that there are Known Folders registered on the system that
  are not represented on this list.
* The module-designated Ids in this list are generated by the module, and the
  assigned symbolic names are derived (by the module author) from those assigned
  to the Known Folder Ids by Microsoft.

[List of KNOWNFOLDERID constants (not up-to-date!) at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/shell/knownfolderid)

### `folderTypeIds`
Constants that correspond to FOLDERTYPEID values available on the current system
according to the installed SDK headers.

*Note: the module-designated type names are derived from the symbolic names
assigned to FOLDERTYPEIDs by Microsoft.*

[List of FOLDERTYPEID constants at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/shell/foldertypeid)

### `listTypes`
Use with function **list** (see above).
* `listTypes.FOLDER_ID`
* `listTypes.CANONICAL_NAME`
* `listTypes.DISPLAY_NAME`
* `listTypes.DETAILS`

### `pathOptions`
Flag values that indicate special fetching instructions for the filesystem path.
Use with functions **getPath** and **getIdList** (see above).
* `pathOptions.API_DEFAULT`
* `pathOptions.DEFAULT_PATH`
* `pathOptions.DONT_VERIFY`
* `pathOptions.NOT_PARENT_RELATIVE`

&gt;= Windows 10, version 1703:
* `pathOptions.REDIRECTION_TARGET`

*Note: These are derived from the [KNOWN_FOLDER_FLAG enumeration.
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/ne-shlobj_core-known_folder_flag)
<br>Not every item on that list is represented in this module.*

### `definitionFlags`
* `definitionFlags.LOCAL_REDIRECT_ONLY`
* `definitionFlags.ROAMABLE`
* `definitionFlags.PRECREATE`

&gt;= Windows 7:
* `definitionFlags.STREAM`
* `definitionFlags.PUBLISH_EXPANDED_PATH`

&gt;= Windows 8.1:
* `definitionFlags.NO_REDIRECT_UI`

[_KF_DEFINITION_FLAGS Enumeration at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_kf_definition_flags)

### `redirectFlags`
* `redirectFlags.ALLOW_ALL`
* `redirectFlags.REDIRECTABLE`
* `redirectFlags.DENY_ALL`
* `redirectFlags.DENY_POLICY_REDIRECTED`
* `redirectFlags.DENY_POLICY`
* `redirectFlags.DENY_PERMISSIONS`

[_KF_REDIRECTION_CAPABILITIES Enumeration at microsoft.com
<svg width="20" height="16">
  <use href="#extLinkSym" stroke="#ccc"></use>
</svg>](https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_kf_redirection_capabilities)

------

**License: MIT**


