# Last Commit Changes
### Added:
- Base Content Mod for the game that contains reference to everything needed (items, blocks, structures, etc)

### Changed:
- Rewrote BaseManager with the new way of loading data from ModContents
- Block structs now cache if they are an Air block, because we no longer have a base id that is considered air
- BaseID struct now compares correctly based on the new class ref that it holds

### Removed:
- Plugins from project folder, they are now in the engine
