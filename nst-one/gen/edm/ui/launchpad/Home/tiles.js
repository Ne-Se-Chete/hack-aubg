/*
 * Generated by Eclipse Dirigible based on model and template.
 *
 * Do not modify the content as it may be re-generated again.
 */
import { response } from "sdk/http";
import { extensions } from "sdk/extensions";
import { user } from "sdk/security";

let tiles = {};

let tileExtensions = await extensions.loadExtensionModules("nst-one-tile");
for (let i = 0; i < tileExtensions?.length; i++) {
    let tile = tileExtensions[i].getTile();

    let hasRoles = true;
    if (tile.roles && Array.isArray(tile.roles)) {
        for (const next of tile.roles) {
            if (!user.isInRole(next)) {
                hasRoles = false;
                break;
            }
        }
    }

    if (!tile || (tile.role && !user.isInRole(tile.role)) || !hasRoles) {
        continue;
    }
    if (!tiles[tile.group]) {
        tiles[tile.group] = {
            entities: [],
            reports: [],
            settings: []
        };
    }
    if (tile.type === 'REPORT' || tile.report === 'true' || tile.report === true) {
        tiles[tile.group].reports.push({
            name: tile.name,
            location: tile.location,
            caption: tile.caption,
            tooltip: tile.tooltip,
            project: tile.project,
            type: tile.type,
            group: tile.group,
            report: tile.report,
            icon: tile.icon,
            order: parseInt(tile.order),
            groupOrder: parseInt(tile.groupOrder)
        });
    } else if (tile.type === 'SETTING') {
        tiles[tile.group].settings.push({
            name: tile.name,
            location: tile.location,
            caption: tile.caption,
            tooltip: tile.tooltip,
            project: tile.project,
            type: tile.type,
            group: tile.group,
            report: tile.report,
            icon: tile.icon,
            order: parseInt(tile.order),
            groupOrder: parseInt(tile.groupOrder)
        });
    } else {
        tiles[tile.group].entities.push({
            name: tile.name,
            location: tile.location,
            caption: tile.caption,
            tooltip: tile.tooltip,
            project: tile.project,
            type: tile.type,
            group: tile.group,
            report: tile.report,
            icon: tile.icon,
            order: parseInt(tile.order),
            groupOrder: parseInt(tile.groupOrder)
        });
    }
}

let modules = [];
let referenceData = [];
for (const [key, group] of Object.entries(tiles)) {
    if (!group.entities.length && !group.reports.length && group.settings.length) {
        group.settings.sort((a, b) => a.order - b.order);
        referenceData.push({ name: key, settings: group.settings });
    } else {
        group.entities.sort((a, b) => a.order - b.order);
        group.reports.sort((a, b) => a.order - b.order);
        group.settings.sort((a, b) => a.order - b.order);
        modules.push({ name: key, ...group });
    }
}
response.setContentType("application/json");
response.println(JSON.stringify({ modules: modules, referenceData: referenceData }));