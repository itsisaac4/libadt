# Ownership and shallow copying

Container storage and resources referenced by an element are two different
kinds of ownership. This matters when a stored structure contains pointers.

The short version:

- The container always owns its raw element storage.
- A shallow copy duplicates bytes, not pointed-to allocations.
- `destroy` describes resources owned by one logical element.
- `Take`, `Pop`, and `Dequeue` transfer ownership instead of destroying it.

## What the container owns

Every container owns the bytes used to store its elements:

- A dynamic array owns its contiguous allocation.
- A linked list owns its nodes and each node's element allocation.
- A stack owns its contiguous allocation.
- A queue owns its nodes and each node's element allocation.

Insertion operations copy an element's bytes into that storage. The source
object itself is never retained.

```c
int value = 10;
da_Append(&array, value);
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

Copying `Student_t` creates a shallow copy: it copies the pointer, not the
string it points to. Both structures temporarily alias the same allocation.
A configured `destroy` callback tells the container how to release resources
owned by the stored element:

```c
static void DestroyStudent(void *element)
{
    Student_t *student = element;
    free(student->name);
    student->name = NULL;
}
```

After a successful append, the stored element becomes responsible for
`student.name`:

```c
Student_t student = {
    .id = 1001,
    .name = CopyString("Ada")
};

if (!ll_Append(&students, &student))
{
    free(student.name);
}
```

Do not free `student.name` after a successful append. The local structure is
now a non-owning alias; ownership has transferred to the stored element.

## Operation ownership rules

| Operation | Resource behavior |
| --- | --- |
| `Append`, `Prepend`, `Insert`, `Push`, `Enqueue` | Copies the element; on success the stored copy assumes responsibility for resources described by `destroy`. |
| `Set` | Destroys resources owned by the replaced element, then stores the shallow replacement copy. |
| `Get`, `Peek`, `Front`, `Back` | Returns a shallow, non-owning copy. The element remains in the container. |
| `Min`, `Max` | Return shallow, non-owning copies. |
| `Remove`, `Discard` | Destroys the removed element's resources. |
| `Take`, `Pop`, `Dequeue` | Removes the element without calling `destroy`; ownership transfers to the output value. |
| `Clear` | Destroys every element but leaves the container initialized. |
| `Destroy` | Destroys every element, releases container storage, and resets the container. |

The rule at the failure boundary is simple: when insertion or replacement
fails, the caller still owns the source element's resources.

## Taking a value out of a container

Use `Take`, `Pop`, or `Dequeue` when the removed element must remain usable:

```c
Student_t transferred = {0};

if (ll_Take(&students, 0, &transferred))
{
    UseStudent(&transferred);
    DestroyStudent(&transferred);
}
```

Calling `Remove` or `Discard` instead would release the element's resources and
would not provide the removed value.

## Avoid duplicate ownership

The most common ownership bug is inserting multiple shallow copies that all
claim the same resource:

```c
Student_t student = MakeStudent(1001, "Ada");
ll_Append(&students, &student);
ll_Append(&students, &student); /* unsafe when both copies destroy name */
```

The two stored structures would contain the same `name` pointer, causing it to
be freed twice. Create a separate deep copy for each owning element.

The same rule applies when inserting an element that is already stored in a
container. The byte copy is safe, but it does not clone anything referenced by
the element.

`InitFrom` also performs shallow copies. If the source elements contain owned
resources and the type has a destroy callback, a successful initialization
makes the container responsible for those resources. The source array must then
be treated as containing non-owning aliases.

## Container lifecycle

The normal lifecycle is:

1. Zero-initialize the container.
2. Call `Init`, `InitFrom`, or its type-inference macro.
3. Use the container.
4. Call `Clear` to reuse it or `Destroy` when finished.

Do not call `Init` again on a live container. It would overwrite the only
references to the existing storage. Call `Destroy` first.

Calling `Destroy` repeatedly is safe after a successful initialization because
the first call resets the container to zero.
