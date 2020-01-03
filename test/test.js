var assert = require('assert')
  , expect = require('chai').expect
  , path = require('path')
  , mod = require('../')

if (process.platform !== 'win32') {
  console.error('This module is only meant for Windows platforms.\n' +
    'Aborting tests.\n');
  return
}

describe('windows-knownfolders module', function() {

  const EXHAUSTIVE = true;

  // Test args to get TypeError thrown
  var nonIntArgs = [ 'dummy', Math.PI, true, [], {} ]
    , nonStringArgs = [ 42, true, [], {}, new Date() ]

  // Reference lists
  // These will have results returned by mod.list(), used throughout the suite:
  var foundIdSet, defList

  var folderTypeValueMap = {}
  for (var typeKey in mod.folderTypeIds)
    folderTypeValueMap[mod.folderTypeIds[typeKey]] = true;

  //---------- Helper functions ----------
  //
  function testErrorCasesById(func, kfId) {
    it('should throw an error when given no argument',
      function() {
        expect(function(){ func() }).to.throw(SyntaxError);
      })

    it('should throw an error when callback argument is omitted',
      function() {
        expect(function(){ func(kfId) }).to.throw(SyntaxError);
      })

    it('should throw an error on invalid type passed for first argument',
      function() {
        for (var i = 0; i < nonIntArgs.length; ++i) {
          expect(function(){ func(nonIntArgs[i], function(){}) })
            .to.throw(TypeError)
        }
      })

    it('should throw an error on non-positive value passed as first argument',
      function() {
        expect(function(){ func(-1, function(){}) }).to.throw(RangeError)
        expect(function(){ func(0, function(){}) }).to.throw(RangeError)
      })
  }

  function isNonnegativeInt(n) {
    // Another way:
    //return (typeof n === "number") && isFinite(n) && (n % 1 === 0) && 0 <= n
    return (typeof n === "number") && isFinite(n) && /^\d\d*$/.test(n.toString())
  }

  // Schema implemented as tests to validate a known folder definition object
  function validateDefSchema(obj) {
    expect(isNonnegativeInt(obj.fid)).to.be.true
    expect(obj.category).to.be.oneOf(["virtual","fixed","common","peruser"])
    expect(obj.name).to.be.a('string').that.is.not.empty
    if (obj.description !== null)
      expect(obj.description).to.be.a('string').that.is.not.empty
    expect(isNonnegativeInt(obj.fidParent)).to.be.true
    if (obj.fidParent !== 0)
      expect(obj.fidParent.toString() in foundIdSet).to.be.true
      // The Microsoft API documentation says that fidParent
      //   "Applies to common and per-user folders only."
      // That is false: The "Fonts" folder has "Windows" as its fidParent,
      // but its category is "fixed"
    if (obj.relativePath !== null)
      expect(obj.relativePath).to.be.a('string').that.is.not.empty
    if (obj.category === 'virtual') {
      expect(obj.relativePath).to.be.null
      // TODO: every virtual known folder I've seen so far has a non-empty
      // parsingName; it might be a requirement for a virtual folder, so
      // this may be needed here:
      //expect(obj.parsingName).to.be.a('string').that.is.not.empty
    }
    if (obj.parsingName !== null) {
      expect(obj.parsingName).to.be.a('string').that.is.not.empty
      // The Microsoft API documentation says that parsingName
      //   "Applies to virtual folders only."
      // That is false: I'm seeing several folders with parsingName that have
      // category == "peruser".
    }
    if (obj.tooltip !== null)
      expect(obj.tooltip).to.be.a('string').that.is.not.empty
    if (obj.localizedName !== null)
      expect(obj.localizedName).to.be.a('string').that.is.not.empty
    if (obj.icon !== null)
      expect(obj.icon).to.be.a('string').that.is.not.empty
    if (obj.security !== null)
      expect(obj.security).to.be.a('string').that.is.not.empty
    expect(isNonnegativeInt(obj.attributes)).to.be.true
    expect(isNonnegativeInt(obj.kfDefFlags)).to.be.true
    expect(isNonnegativeInt(obj.folderTypeId)).to.be.true
    expect(obj.folderTypeId.toString() in folderTypeValueMap).to.be.true
  }
  //---------- END Helper functions ----------

  var funcNames = [
     'list', 'findFromPath', 'getByName', 'getPath', 'getCanonicalName',
     'getDisplayName', 'getCategory', 'getFolderType', 'getDefinition',
     'getIdList', 'getRedirectionCapabilities',
     'getIdName', 'getIdTypeName'
  ]

  it('should export these functions: ' + funcNames.join(', '),
    function() {
      for (var i in funcNames)
        expect(mod[funcNames[i]]).to.be.a('function')
    })

  var constantSets = [
    'folderIds', 'folderTypeIds', 'listTypes', 'pathOptions',
    'definitionFlags', 'redirectFlags'
  ]

  it('should export these sets of distinct numeric constants: '
     + constantSets.join(', '),
    function() {
      var symName, set, iterated

      for (var setName in constantSets) {
        iterated = {}
        set = mod[constantSets[setName]]
        expect(set).to.be.an('object')

        for (symName in set)
          expect(set[symName]).to.be.a('number')

        for (symName in set) { // Each value is distinct from the others
          iterated[symName] = true
          for (var otherName in set) {
            if (!(otherName in iterated)) // No need to check already checked
              expect(set[symName]).to.not.equal(set[otherName])
          }
        }

        for (symName in set)   // Prove that each is constant
          expect(function(){ set[symName]++ }).to.not.change(set, symName)
      }
    })

  var highestCompileTimeId = 0
  for (var key in mod.folderIds) {
    if (mod.folderIds[key] > highestCompileTimeId)
      highestCompileTimeId = mod.folderIds[key]
  }
  // Now we are able to compare fetched Ids to the highest one known at
  // compile time; if the fetched one is higher, then we expect getIdName
  // to return an empty string.

  describe('getIdName function', function() {

    it('should return a unique non-empty string for each folderIds constant',
      function() {
        var name
        for (var key in mod.folderIds) {
          name = mod.getIdName(mod.folderIds[key])
          expect(name).to.be.a('string').and.to.equal(key)
        }
      })

    it('should throw an error when given no argument',
      function() {
        expect(function(){ mod.getIdName() }).to.throw(SyntaxError);
      })

    it('should throw an error when given non-numeric argument',
      function() {
        expect(function(){ mod.getIdName(undefined) }).to.throw(SyntaxError);
        expect(function(){ mod.getIdName(null) }).to.throw(SyntaxError);

        for (var i = 0; i < nonIntArgs.length; ++i) {
          expect(function(){ mod.getIdName(nonIntArgs[i]) })
            .to.throw(TypeError)
        }
      })

    it('should throw errors when given out-of-range numbers',
      function() {
        expect(function(){ mod.getIdName(-1) }).to.throw(RangeError);
        expect(function(){ mod.getIdName(highestCompileTimeId + 1) })
          .to.throw(RangeError);
      })

  })

  describe('getIdTypeName function', function() {

    it('should return a unique non-empty string for each folderTypeIds constant',
      function() {
        var name
        for (var key in mod.folderTypeIds) {
          name = mod.getIdTypeName(mod.folderTypeIds[key])
          expect(name).to.be.a('string').and.to.equal(key)
        }
      })

    it('should throw an error when given no argument',
      function() {
        expect(function(){ mod.getIdTypeName() }).to.throw(SyntaxError);
      })

    it('should throw an error when given non-numeric argument',
      function() {
        expect(function(){ mod.getIdTypeName(undefined) }).to.throw(SyntaxError);
        expect(function(){ mod.getIdTypeName(null) }).to.throw(SyntaxError);

        for (var i = 0; i < nonIntArgs.length; ++i) {
          expect(function(){ mod.getIdTypeName(nonIntArgs[i]) })
            .to.throw(TypeError)
        }
      })

    it('should throw an error when given an out-of-range number',
      function() {
        var tooHighForTypeId = Object.keys(mod.folderTypeIds).length
        expect(function(){ mod.getIdTypeName(-1) }).to.throw(RangeError);
        expect(function(){ mod.getIdTypeName(tooHighForTypeId) })
          .to.throw(RangeError);
      })
  })

  describe('list function', function() {

    // Helper functions ---------------------------------------------------
    function verifyUniqueIds(list, remember) {
      var seen = {}
      for (var i = 0; i < list.length; ++i) {
        expect(list[i]).to.be.a('number').that.is.above(0)
        expect(seen[list[i]]).to.be.undefined
        seen[list[i]] = true
      }
      if (remember) foundIdSet = seen
    }

    function verifyUniqueNames(list) {
      var seen = {}
      for (var i = 0; i < list.length; ++i) {
        expect(list[i]).to.be.a('string').that.is.not.empty
        expect(seen[list[i]]).to.be.undefined
        seen[list[i]] = true
      }
    }
    // END Helper functions ------------------------------------------------

    it('should throw an error when given no argument',
      function() {
        expect(function(){ mod.list() }).to.throw(SyntaxError);
      })

    it('should throw an error when callback argument is omitted',
      function() {
        expect(
          function(){ mod.list(mod.listTypes.FOLDER_ID) }
        ).to.throw(SyntaxError);
      })

    it('should throw an error when invalid type passed for first argument',
      function() {
        for (var i = 0; i < nonIntArgs.length; ++i) {
          expect(function(){ mod.list(nonIntArgs[i], function(){}) })
            .to.throw(TypeError)
        }
      })

    it('should pass back an array of unique numbers when given no option argument',
      function(done) {
        mod.list(function(err, data) {
          expect(err).to.be.null
          expect(data).to.be.instanceof(Array)
          if (data.length == 0) {
            console.warn(
              'NO KNOWN FOLDERS REGISTERED ON THIS SYSTEM!\n' +
              'NO MEANINGFUL TESTS CAN BE DONE, SO TEST SUITE WILL BE ABORTED.\n' +
              'SORRY!'
            );
            process.exit()
          }
          verifyUniqueIds(data, true) // true: side effect -> keep map of Ids
          done()
        })
      })

    it('should pass back an array of unique numbers when given option '
       + 'listTypes.FOLDER_ID',
      function(done) {
        mod.list(mod.listTypes.FOLDER_ID, function(err, data) {
          expect(err).to.be.null
          expect(data).to.be.instanceof(Array)
          expect(data).to.not.be.empty
          verifyUniqueIds(data)
          done()
        })
      })

    it('should pass back an array of objects {name, fid} when given option '
       + 'listTypes.CANONICAL_NAME',
      function(done) {
        mod.list(mod.listTypes.CANONICAL_NAME, function(err, data) {
          expect(err).to.be.null
          expect(data).to.be.instanceof(Array)
          verifyUniqueNames(data.map(function(el){ return el.name }))
          verifyUniqueIds(data.map(function(el){ return el.fid }))
          done()
        })
      })

    it('should pass back an array of objects {name, fid} when given option '
       + 'listTypes.DISPLAY_NAME',
      function(done) {
        mod.list(mod.listTypes.DISPLAY_NAME, function(err, data) {
          expect(err).to.be.null
          expect(data).to.be.instanceof(Array)
          // We can't call verifyUniqueNames() on the data, because
          // display names are not unique! In fact, many entries are
          // passed back with null name, because the folder has none (virtual?)
          for (var i = 0; i < data.length; ++i) {
            if (data[i].name !== null)
              expect(data[i].name).to.be.a('string').that.is.not.empty
          }
          verifyUniqueIds(data.map(function(el){ return el.fid }))
          done()
        })
      })

    it('should pass back an array of definition objects when given option '
       + 'listTypes.DETAILS',
      function(done) {
        mod.list(mod.listTypes.DETAILS, function(err, data) {
          expect(err).to.be.null
          expect(data).to.be.instanceof(Array)
          for (var i = 0; i < data.length; ++i) {
            expect(data[i].fid).to.be.a('number').that.is.above(0)
            expect(data[i].fid.toString() in foundIdSet)
            validateDefSchema(data[i])
          }
          verifyUniqueIds(data.map(function(el){ return el.fid }))

          // Save the definitions for later use with getDefinition testing
          defList = data
          done()
          describe('', doTasksThatRequireTheLists)
        })
      })
  })

  function doTasksThatRequireTheLists() {

    describe('getPath function', function() {

      var validKFId = mod.folderIds.Windows

      testErrorCasesById(mod.getPath, validKFId)

      // More error cases, because this also takes an optional argument
      it('should throw an error when invalid type passed for 2nd argument',
        function() {
          for (var i = 0; i < nonIntArgs.length; ++i) {
            expect(
              function(){ mod.getPath(validKFId, nonIntArgs[i], function(){}) }
            ).to.throw(TypeError)
          }
        })

      it('should throw an error when invalid value passed for 2nd argument',
        function() {
          var badOptVal = 1
          // Calculate a value that is out of range for pathOption by adding all
          // the known flags to initial value of 1
          for (var key in mod.pathOptions) badOptVal += mod.pathOptions[key]

          expect(function(){ mod.getPath(validKFId, badOptVal, function(){}) })
            .to.throw(RangeError)
          expect(function(){ mod.getPath(validKFId, -1, function(){}) })
            .to.throw(RangeError)
        })

      function validatePathString(p) {
        expect(p).to.be.a('string').that.is.not.empty
        var parseObj = path.parse(p)
        // Because it's supposed to be an absolute path:
        expect(parseObj.root).to.not.be.empty
        expect(parseObj.base).to.not.be.empty
      }

      it('should pass back a parseable path when given the Id of an existing '
         + 'known folder',
        function(done) {
          mod.getPath(validKFId, function(err, p) {
            expect(err).to.be.null
            validatePathString(p)
            done()
          })
        })

      it('should pass back a parseable path when given the Id of an existing '
         + 'known folder and a valid pathOption value',
        function(done) {
          var opts = mod.pathOptions
          var optCombos = [
            opts.API_DEFAULT,
            opts.DEFAULT_PATH,
            opts.DEFAULT_PATH | opts.DONT_VERIFY,
            opts.DEFAULT_PATH | opts.NOT_PARENT_RELATIVE,
            opts.DEFAULT_PATH | opts.NOT_PARENT_RELATIVE | opts.DONT_VERIFY
          ]
          if ('REDIRECTION_TARGET' in mod.pathOptions)
            optCombos.push( opts.REDIRECTION_TARGET )

          function iterateOptsGetPath(currIdx) {
            mod.getPath(validKFId, optCombos[currIdx], function(err, p) {
              expect(err).to.be.null
              validatePathString(p)
              if (++currIdx < optCombos.length)
                return iterateOptsGetPath(currIdx)

              done()
            })
          }
          iterateOptsGetPath(0)
        })
    })

    describe('findFromPath function', function() {

      this.timeout(5000);

      it('should throw an error when given no argument',
        function() {
          expect(function(){ mod.findFromPath() }).to.throw(SyntaxError);
        })

      it('should throw an error when callback argument is omitted',
        function() {
          expect(function(){ mod.findFromPath("dummyPath") })
            .to.throw(SyntaxError);
        })

      it('should throw an error on invalid type passed for first argument',
        function() {
          for (var i = 0; i < nonStringArgs.length; ++i) {
            expect(function(){ mod.findFromPath(nonStringArgs[i], function(){}) })
              .to.throw(TypeError)
          }
        })

      // WARNING: Although properties like the folder Id and the canonical name
      // are unique to a known folder registration, the absolute path does not
      // have to be!
      // Therefore, it would be naive to call getPath(kfId) to fetch resultPath,
      // follow that with a call to findFromPath(resultPath) to fetch resultId,
      // and then expect that resultId == kfId.
      // Maybe it will, maybe it won't.
      // (Case in point: 'My Pictures' and 'Local Pictures' are found to have
      // the same path; but findFromPath() can only fetch one result.)
      //
      // STRATEGY:
      // 1. For each kfId in the fetched list, call getPath(kfId) and add kfId
      //    to the list of Ids associated with the result path.
      // 2. For each path in the map, call findFromPath(kfPath) and expect the
      //    fetched Id to be in the list mapped to kfPath.

      it('should pass back a folder Id when given the path registered to it',
        function(done) {

          var mapPathToId = {}
          var idx = 0

          ;(function matchIdsToPaths() {
            var kfId = defList[idx].fid
            mod.getPath(kfId, function(err, fetchedPath) {
              expect(err).to.be.null
              if (fetchedPath) {
                if (!(fetchedPath in mapPathToId)) {
                  mapPathToId[fetchedPath] = []
                }
                mapPathToId[fetchedPath].push(kfId)
              }
              if (++idx < defList.length)
                return matchIdsToPaths()

              var absPaths = Object.keys(mapPathToId)
              idx = 0
              ;(function checkPathMatch() {
                var tgtPath = absPaths[idx]
                mod.findFromPath(tgtPath, function(err, fetchedId) {
                  expect(err).to.be.null
                  expect(fetchedId).to.be.oneOf(mapPathToId[tgtPath])

                  if (++idx < absPaths.length) {
                    checkPathMatch()
                  }
                  else done()
                })
              })()
            })
          })()
        })
    })

    describe('getCanonicalName function', function() {

      var validKFId = mod.folderIds.Profile
      if (EXHAUSTIVE) this.timeout(5000);

      testErrorCasesById(mod.getCanonicalName, validKFId)

      it('should pass back a non-empty string when given the Id of an existing '
         + 'known folder',
        function(done) {
          var defIdx = 0;

          (function runTestCases() {

            var kfId = EXHAUSTIVE ? defList[defIdx].fid : validKFId

            mod.getCanonicalName(kfId, function(err, name) {
              expect(err).to.be.null
              expect(name).to.be.a('string').that.is.not.empty

              if (EXHAUSTIVE && ++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })
    })

    describe('getByName function', function() {

      if (EXHAUSTIVE) this.timeout(5000)

      it('should throw an error when given no argument',
        function() {
          expect(function(){ mod.getByName() }).to.throw(SyntaxError);
        })

      it('should throw an error when callback argument is omitted',
        function() {
          expect(function(){ mod.getByName("Profile") })
            .to.throw(SyntaxError);
        })

      it('should throw an error on invalid type passed for first argument',
        function() {
          for (var i = 0; i < nonStringArgs.length; ++i) {
            expect(function(){ mod.getByName(nonStringArgs[i], function(){}) })
              .to.throw(TypeError)
          }
        })

      it('should pass back the same folder Id used to retrieve a canonical name '
         + 'when given that same name',
        function(done) {

          var defIdx = 0

          ;(function runTestCases() {

            var kfId = EXHAUSTIVE ? defList[defIdx].fid : mod.folderIds.Profile
            var tgtName = EXHAUSTIVE ? defList[defIdx].name : "Profile"
            mod.getByName(tgtName, function(err, fetchedId) {
              expect(err).to.be.null
              expect(fetchedId).to.equal(kfId)
              if (EXHAUSTIVE && ++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })
    })

    describe('getDisplayName function', function() {

      var validKFId = mod.folderIds.Windows
      if (EXHAUSTIVE) this.timeout(5000)

      testErrorCasesById(mod.getDisplayName, validKFId)

      it('should pass back a non-empty string or null when given the Id '
         + ' of an existing known folder',
        function(done) {
          var defIdx = 0

          ;(function runTestCases() {

            var kfId = EXHAUSTIVE ? defList[defIdx].fid : validKFId

            mod.getDisplayName(validKFId, function(err, name) {
              expect(err).to.be.null
              if (name !== null) {
                expect(name).to.be.a('string').that.is.not.empty
              }

              if (EXHAUSTIVE && ++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })
    })

    describe('getCategory function', function() {

      var validKFId = mod.folderIds.Profile
      if (EXHAUSTIVE) this.timeout(5000)

      testErrorCasesById(mod.getCategory, validKFId)

      it('should pass back one of "virtual", "fixed", "common", or "peruser" '
         + 'when given the Id of an existing known folder',
        function(done) {
          var defIdx = 0

          ;(function runTestCases() {

            var kfId = EXHAUSTIVE ? defList[defIdx].fid : validKFId

            mod.getCategory(validKFId, function(err, cat) {
              expect(err).to.be.null
              expect(cat).to.be.a('string').that.is.not.empty
              expect(cat).to.be.oneOf(["virtual", "fixed", "common", "peruser"])

              if (EXHAUSTIVE && ++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })
    })

    describe('getFolderType function', function() {

      var validKFId = mod.folderIds.Profile
      if (EXHAUSTIVE) this.timeout(5000)

      testErrorCasesById(mod.getFolderType, validKFId)

      it('should pass back a valid Folder Type Id when given the Id of an '
         + 'existing known folder',
        function(done) {
          var defIdx = 0

          ;(function runTestCases() {

            var kfId = EXHAUSTIVE ? defList[defIdx].fid : validKFId
            var expectedNum = EXHAUSTIVE ? defList[defIdx].folderTypeId : 0

            mod.getFolderType(kfId, function(err, typeNum) {
              var typeIdListLength = Object.values(mod.folderTypeIds).length;
              expect(err).to.be.null
              expect(typeNum).to.equal(expectedNum);
              // Note: we already validated the 'folderTypeId' property of each
              // folder definition in the list() test above; therefore, as long
              // as the current result equals what we previously fetched, we
              // don't need to check anything else.

              if (EXHAUSTIVE && ++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })
    })

    describe('getDefinition function', function() {
      // Note that we have already retrieved definitions for all Known Folders
      // on the current system, and validated each versus our schema, in the
      // test of list() with option listTypes.DETAILS above.

      // This test will be exhaustive regardless of EXHAUSTIVE flag setting
      this.timeout(5000)
      testErrorCasesById(mod.getDefinition, mod.folderIds.Windows)

      it('should pass back the same data for a given Id as collected by list()',
        function(done) {
          var defIdx = 0

          ;(function runTestCases() {

            var refObject = defList[defIdx]

            mod.getDefinition(refObject.fid, function(err, fetched) {
              expect(err).to.be.null
              expect(fetched).to.deep.equal(refObject)

              if (++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })
    })

    describe('getIdList function', function() {

      var validKFId = mod.folderIds.Profile
      if (EXHAUSTIVE) this.timeout(5000)

      testErrorCasesById(mod.getIdList, validKFId)

      it('should pass back a buffer of data that conforms to the ITEMIDLIST '
         + 'structure when given the Id of an existing known folder',
        function(done) {
          var defIdx = 0

          ;(function runTestCases() {

            var kfId = EXHAUSTIVE ? defList[defIdx].fid : validKFId

            mod.getIdList(validKFId, function(err, buf) {
              expect(err).to.be.null
              expect(buf).to.be.an.instanceof(Buffer).that.is.not.empty

              var offset = 0
              var byteCount
              while (byteCount = buf.readUInt16LE(offset)) {
                buf.slice(offset+2, offset+byteCount)
                offset += byteCount
              }
              expect(byteCount).to.equal(0)
              expect(offset).to.equal(buf.length - 2)

              if (EXHAUSTIVE && ++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })

    })

    describe('getRedirectionCapabilities function', function() {

      var validKFId = mod.folderIds.Profile
      if (EXHAUSTIVE) this.timeout(5000)

      testErrorCasesById(mod.getRedirectionCapabilities, validKFId)

      it('should pass back a valid combination of redirectFlags when given the '
         + 'Id of an existing known folder',
        function(done) {
          var defIdx = 0

          ;(function runTestCases() {

            var kfId = EXHAUSTIVE ? defList[defIdx].fid : validKFId

            mod.getRedirectionCapabilities(validKFId, function(err, data) {
              var testMask
              expect(err).to.be.null

              testMask = mod.redirectFlags.ALLOW_ALL | mod.redirectFlags.DENY_ALL
              expect(data & testMask).to.equal(data)

              if (EXHAUSTIVE && ++defIdx < defList.length) {
                runTestCases()
              }
              else done()
            })
          })()
        })
    })
  }
})
