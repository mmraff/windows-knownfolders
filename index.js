var addon = require('bindings')('winknownfolders')

var prop, propType
// addon.path might prove useful, but we don't want to export it
for (prop in addon) {
  propType = typeof addon[prop]
  if ('function' === propType || 'object' === propType)
    module.exports[prop] = addon[prop]
}
