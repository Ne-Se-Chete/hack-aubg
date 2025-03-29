import { query } from "sdk/db";
import { producer } from "sdk/messaging";
import { extensions } from "sdk/extensions";
import { dao as daoApi } from "sdk/db";

export interface ReadingEntity {
    readonly Id: number;
    Property?: number;
    Value?: number;
    Timestamp?: Date;
}

export interface ReadingCreateEntity {
    readonly Property?: number;
    readonly Value?: number;
    readonly Timestamp?: Date;
}

export interface ReadingUpdateEntity extends ReadingCreateEntity {
    readonly Id: number;
}

export interface ReadingEntityOptions {
    $filter?: {
        equals?: {
            Id?: number | number[];
            Property?: number | number[];
            Value?: number | number[];
            Timestamp?: Date | Date[];
        };
        notEquals?: {
            Id?: number | number[];
            Property?: number | number[];
            Value?: number | number[];
            Timestamp?: Date | Date[];
        };
        contains?: {
            Id?: number;
            Property?: number;
            Value?: number;
            Timestamp?: Date;
        };
        greaterThan?: {
            Id?: number;
            Property?: number;
            Value?: number;
            Timestamp?: Date;
        };
        greaterThanOrEqual?: {
            Id?: number;
            Property?: number;
            Value?: number;
            Timestamp?: Date;
        };
        lessThan?: {
            Id?: number;
            Property?: number;
            Value?: number;
            Timestamp?: Date;
        };
        lessThanOrEqual?: {
            Id?: number;
            Property?: number;
            Value?: number;
            Timestamp?: Date;
        };
    },
    $select?: (keyof ReadingEntity)[],
    $sort?: string | (keyof ReadingEntity)[],
    $order?: 'asc' | 'desc',
    $offset?: number,
    $limit?: number,
}

interface ReadingEntityEvent {
    readonly operation: 'create' | 'update' | 'delete';
    readonly table: string;
    readonly entity: Partial<ReadingEntity>;
    readonly key: {
        name: string;
        column: string;
        value: number;
    }
}

interface ReadingUpdateEntityEvent extends ReadingEntityEvent {
    readonly previousEntity: ReadingEntity;
}

export class ReadingRepository {

    private static readonly DEFINITION = {
        table: "READING",
        properties: [
            {
                name: "Id",
                column: "READING_ID",
                type: "INTEGER",
                id: true,
                autoIncrement: true,
            },
            {
                name: "Property",
                column: "READING_PROPERTY",
                type: "INTEGER",
            },
            {
                name: "Value",
                column: "READING_VALUE",
                type: "INTEGER",
            },
            {
                name: "Timestamp",
                column: "READING_TIMESTAMP",
                type: "TIMESTAMP",
            }
        ]
    };

    private readonly dao;

    constructor(dataSource = "DefaultDB") {
        this.dao = daoApi.create(ReadingRepository.DEFINITION, null, dataSource);
    }

    public findAll(options?: ReadingEntityOptions): ReadingEntity[] {
        return this.dao.list(options);
    }

    public findById(id: number): ReadingEntity | undefined {
        const entity = this.dao.find(id);
        return entity ?? undefined;
    }

    public create(entity: ReadingCreateEntity): number {
        const id = this.dao.insert(entity);
        this.triggerEvent({
            operation: "create",
            table: "READING",
            entity: entity,
            key: {
                name: "Id",
                column: "READING_ID",
                value: id
            }
        });
        return id;
    }

    public update(entity: ReadingUpdateEntity): void {
        const previousEntity = this.findById(entity.Id);
        this.dao.update(entity);
        this.triggerEvent({
            operation: "update",
            table: "READING",
            entity: entity,
            previousEntity: previousEntity,
            key: {
                name: "Id",
                column: "READING_ID",
                value: entity.Id
            }
        });
    }

    public upsert(entity: ReadingCreateEntity | ReadingUpdateEntity): number {
        const id = (entity as ReadingUpdateEntity).Id;
        if (!id) {
            return this.create(entity);
        }

        const existingEntity = this.findById(id);
        if (existingEntity) {
            this.update(entity as ReadingUpdateEntity);
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
            table: "READING",
            entity: entity,
            key: {
                name: "Id",
                column: "READING_ID",
                value: id
            }
        });
    }

    public count(options?: ReadingEntityOptions): number {
        return this.dao.count(options);
    }

    public customDataCount(): number {
        const resultSet = query.execute('SELECT COUNT(*) AS COUNT FROM "READING"');
        if (resultSet !== null && resultSet[0] !== null) {
            if (resultSet[0].COUNT !== undefined && resultSet[0].COUNT !== null) {
                return resultSet[0].COUNT;
            } else if (resultSet[0].count !== undefined && resultSet[0].count !== null) {
                return resultSet[0].count;
            }
        }
        return 0;
    }

    private async triggerEvent(data: ReadingEntityEvent | ReadingUpdateEntityEvent) {
        const triggerExtensions = await extensions.loadExtensionModules("nst-one-entities-Reading", ["trigger"]);
        triggerExtensions.forEach(triggerExtension => {
            try {
                triggerExtension.trigger(data);
            } catch (error) {
                console.error(error);
            }            
        });
        producer.topic("nst-one-entities-Reading").send(JSON.stringify(data));
    }
}
