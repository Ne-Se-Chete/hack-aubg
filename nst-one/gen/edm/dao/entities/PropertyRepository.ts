import { query } from "sdk/db";
import { producer } from "sdk/messaging";
import { extensions } from "sdk/extensions";
import { dao as daoApi } from "sdk/db";

export interface PropertyEntity {
    readonly Id: number;
    Name?: string;
}

export interface PropertyCreateEntity {
    readonly Name?: string;
}

export interface PropertyUpdateEntity extends PropertyCreateEntity {
    readonly Id: number;
}

export interface PropertyEntityOptions {
    $filter?: {
        equals?: {
            Id?: number | number[];
            Name?: string | string[];
        };
        notEquals?: {
            Id?: number | number[];
            Name?: string | string[];
        };
        contains?: {
            Id?: number;
            Name?: string;
        };
        greaterThan?: {
            Id?: number;
            Name?: string;
        };
        greaterThanOrEqual?: {
            Id?: number;
            Name?: string;
        };
        lessThan?: {
            Id?: number;
            Name?: string;
        };
        lessThanOrEqual?: {
            Id?: number;
            Name?: string;
        };
    },
    $select?: (keyof PropertyEntity)[],
    $sort?: string | (keyof PropertyEntity)[],
    $order?: 'asc' | 'desc',
    $offset?: number,
    $limit?: number,
}

interface PropertyEntityEvent {
    readonly operation: 'create' | 'update' | 'delete';
    readonly table: string;
    readonly entity: Partial<PropertyEntity>;
    readonly key: {
        name: string;
        column: string;
        value: number;
    }
}

interface PropertyUpdateEntityEvent extends PropertyEntityEvent {
    readonly previousEntity: PropertyEntity;
}

export class PropertyRepository {

    private static readonly DEFINITION = {
        table: "PROPERTY",
        properties: [
            {
                name: "Id",
                column: "PROPERTY_ID",
                type: "INTEGER",
                id: true,
                autoIncrement: true,
            },
            {
                name: "Name",
                column: "PROPERTY_NAME",
                type: "VARCHAR",
            }
        ]
    };

    private readonly dao;

    constructor(dataSource = "DefaultDB") {
        this.dao = daoApi.create(PropertyRepository.DEFINITION, null, dataSource);
    }

    public findAll(options?: PropertyEntityOptions): PropertyEntity[] {
        return this.dao.list(options);
    }

    public findById(id: number): PropertyEntity | undefined {
        const entity = this.dao.find(id);
        return entity ?? undefined;
    }

    public create(entity: PropertyCreateEntity): number {
        const id = this.dao.insert(entity);
        this.triggerEvent({
            operation: "create",
            table: "PROPERTY",
            entity: entity,
            key: {
                name: "Id",
                column: "PROPERTY_ID",
                value: id
            }
        });
        return id;
    }

    public update(entity: PropertyUpdateEntity): void {
        const previousEntity = this.findById(entity.Id);
        this.dao.update(entity);
        this.triggerEvent({
            operation: "update",
            table: "PROPERTY",
            entity: entity,
            previousEntity: previousEntity,
            key: {
                name: "Id",
                column: "PROPERTY_ID",
                value: entity.Id
            }
        });
    }

    public upsert(entity: PropertyCreateEntity | PropertyUpdateEntity): number {
        const id = (entity as PropertyUpdateEntity).Id;
        if (!id) {
            return this.create(entity);
        }

        const existingEntity = this.findById(id);
        if (existingEntity) {
            this.update(entity as PropertyUpdateEntity);
            return id;
        } else {
            return this.create(entity);
        }
    }

    public deleteById(id: number): void {
        const entity = this.dao.find(id);
        this.dao.remove(id);
        this.triggerEvent({
            operation: "delete",
            table: "PROPERTY",
            entity: entity,
            key: {
                name: "Id",
                column: "PROPERTY_ID",
                value: id
            }
        });
    }

    public count(options?: PropertyEntityOptions): number {
        return this.dao.count(options);
    }

    public customDataCount(): number {
        const resultSet = query.execute('SELECT COUNT(*) AS COUNT FROM "PROPERTY"');
        if (resultSet !== null && resultSet[0] !== null) {
            if (resultSet[0].COUNT !== undefined && resultSet[0].COUNT !== null) {
                return resultSet[0].COUNT;
            } else if (resultSet[0].count !== undefined && resultSet[0].count !== null) {
                return resultSet[0].count;
            }
        }
        return 0;
    }

    private async triggerEvent(data: PropertyEntityEvent | PropertyUpdateEntityEvent) {
        const triggerExtensions = await extensions.loadExtensionModules("nst-one-entities-Property", ["trigger"]);
        triggerExtensions.forEach(triggerExtension => {
            try {
                triggerExtension.trigger(data);
            } catch (error) {
                console.error(error);
            }            
        });
        producer.topic("nst-one-entities-Property").send(JSON.stringify(data));
    }
}
