# Ownership and shallow copying

libadt separates container storage from resources referenced by an element.
Understanding that distinction is essential when storing structures that
contain pointers.

## What the container owns

Every container owns the bytes used to store its elements:

- A dynamic array owns its contiguous allocation.
- A linked list owns its nodes and each node's element allocation.

Insertion operations copy an element's bytes into that storage. The source
object itself is never retained.

```c
int value = 10;
da_AppendRef(&array, &value);
value = 20;
```

The stored value remains `10` because the integer bytes were copied.

## Resources owned by elements

An element may contain a pointer to another allocation:

```c
typedef struct
{
    int id;
    char *name;
} Student_t;
```

Copying `Student_t` copies the pointer, not the string it points to. The
container does not know how to clone that string. A configured `destroy`
callback tells the container how to release resources owned by a stored
element:

```c
static void DestroyStudent(void *element)
{
    Student_t *student = element;
    free(student->name);
    student->name = NULL;
}
```

After a successful append, ownership of `student.name` is logically handed to
the stored element:

```c
Student_t student = {
    .id = 1001,
    .name = CopyString("Ada")
};

if (!ll_AppendRef(&students, &student))
{
    free(student.name);
}
```

Do not free `student.name` after a successful append. The local structure still
contains a shallow alias, but the container's stored element is now responsible
for releasing the allocation.

## Operation ownership rules

| Operation | Resource behavior |
| --- | --- |
| `AppendRef`, `PrependRef`, `InsertRef` | Copies the element; on success the stored copy assumes responsibility for resources described by `destroy`. |
| `SetRef` | Destroys resources owned by the replaced element, then stores the shallow replacement copy. |
| `Get` | Returns a shallow, non-owning copy. The element remains in the container. |
| `Min`, `Max` | Return shallow, non-owning copies. |
| `Remove` | Destroys the removed element's resources. |
| `Take` | Removes the element without calling `destroy`; ownership transfers to the output value. |
| `Clear` | Destroys every element but leaves the container initialized. |
| `Destroy` | Destroys every element, releases container storage, and resets the container. |

When an insertion or replacement operation fails, the caller retains
responsibility for the source element's resources.

## Taking ownership out of a container

Use `Take` when the removed element must remain usable:

```c
Student_t transferred = {0};

if (ll_Take(&students, 0, &transferred))
{
    UseStudent(&transferred);
    DestroyStudent(&transferred);
}
```

Calling `Remove` instead would release `transferred` resources before returning
and would not provide the removed element.

## Avoid duplicate ownership

Do not insert multiple shallow copies that all claim ownership of the same
resource:

```c
Student_t student = MakeStudent(1001, "Ada");
ll_AppendRef(&students, &student);
ll_AppendRef(&students, &student); /* unsafe when both copies destroy name */
```

The two stored structures would contain the same `name` pointer, causing it to
be freed twice. Create a separate deep copy for each owning element.

The same rule applies when inserting an element already stored inside a
container. Internal-source insertion is safe for primitives and non-owning
structures, but it does not clone pointed-to resources.

`InitFrom` also performs shallow copies. If the source elements contain owned
resources and the type has a destroy callback, a successful initialization
makes the container responsible for those resources. The source array must then
be treated as containing non-owning aliases.

## Container lifecycle

Use this lifecycle:

1. Zero-initialize the container.
2. Call `Init`, `InitFrom`, or its type-inference macro.
3. Use the container.
4. Call `Clear` to reuse it or `Destroy` when finished.

Do not call `Init` again on a live container. Initialization replaces the
container fields and cannot release storage whose address it is about to
overwrite. Call `Destroy` first.

Calling `Destroy` repeatedly is safe after a successful initialization because
the first call resets the container to zero.
